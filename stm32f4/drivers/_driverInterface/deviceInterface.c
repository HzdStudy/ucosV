#include "deviceInterface.h"
#if 0
#define _OBJ_CONTAINER_LIST_INIT(c)     \
    {&(object_container[c].object_list), &(object_container[c].object_list)}
struct object_information object_container[Object_Class_Unknown] =
{
	/* initialize object container - device */
    {Object_Class_Device, _OBJ_CONTAINER_LIST_INIT(Object_Class_Device), sizeof(struct device)},
};

#endif

static list_t     device_list;              /**< object list */



/**
 * This function will initialize an object and add it to object system
 * management.
 *
 * @param object the specified object to be initialized.
 * @param type the object type.
 * @param name the object name. In system, the object's name must be unique.
 */
static void device_listadd(struct object         *object,
                                const char               *name)
{
    register base_t temp;

    /* copy name */
    strcpy(object->name, name);

    /* insert object into information object list */
    list_insert_after(&(device_list), &(object->list));
}


/**
 * This function will detach a static object from object system,
 * and the memory of static object is not freed.
 *
 * @param object the specified object to be detached.
 */
static void device_detach(object_t object)
{
    register base_t temp;

    /* remove from old list */
    list_remove(&(object->list));

}


/**
 * This function registers a device driver with specified name.
 *
 * @param dev the pointer of device driver structure
 * @param name the device driver's name
 * @param flags the flag of device
 *
 * @return the error code, EOK on initialization successfully.
 */
err_t device_register(device_t dev,
                            const char *name,
                            uint16_t flags)
{
    if (dev == NULL)
        return -ERROR;

    if (device_find(name) != NULL)
        return -ERROR;

    device_listadd(&(dev->parent), name);
    dev->flag = flags;
    dev->ref_count = 0;

    return EOK;
}

/**
 * This function removes a previously registered device driver
 *
 * @param dev the pointer of device driver structure
 *
 * @return the error code, RT_EOK on successfully.
 */
err_t device_unregister(device_t dev)
{

    device_detach(&(dev->parent));

    return EOK;
}
							

/**
 * This function will open a device
 *
 * @param dev the pointer of device driver structure
 * @param oflag the flags for device open
 *
 * @return the result
 */
err_t device_open(device_t dev, uint16_t oflag)
{
    err_t result = EOK;

    //ASSERT(dev != NULL);

    /* if device is not initialized, initialize it. */
    if (!(dev->flag & DEVICE_FLAG_ACTIVATED))
    {
        if (dev->init != NULL)
        {
            result = dev->init(dev);
            if (result != EOK)
            {
                printf("To initialize device:%s failed. The error code is %d\n",
                           dev->parent.name, result);

                return result;
            }
        }

        dev->flag |= DEVICE_FLAG_ACTIVATED;
    }

    /* device is a stand alone device and opened */
    if ((dev->flag & DEVICE_FLAG_STANDALONE) &&
        (dev->open_flag & DEVICE_OFLAG_OPEN))
    {
        return -EBUSY;
    }

    /* call device open interface */
    if (dev->open != NULL)
    {
        result = dev->open(dev, oflag);
    }

    /* set open flag */
    if (result == EOK || result == -ENOSYS)
    {
        dev->open_flag = oflag | DEVICE_OFLAG_OPEN;

        dev->ref_count++;
        /* don't let bad things happen silently. If you are bitten by this assert,
         * please set the ref_count to a bigger type. */
       // ASSERT(dev->ref_count != 0);
    }

    return result;
}

/**
 * This function will close a device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
err_t device_close(device_t dev)
{
    err_t result = EOK;


    if (dev->ref_count == 0)
        return -ERROR;

    dev->ref_count--;

    if (dev->ref_count != 0)
        return EOK;

    /* call device close interface */
    if (dev->close != NULL)
    {
        result = dev->close(dev);
    }

    /* set open flag */
    if (result == EOK || result == -ENOSYS)
        dev->open_flag = DEVICE_OFLAG_CLOSE;

    return result;
}


/**
 * This function will read some data from a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of reading
 * @param buffer the data buffer to save read data
 * @param size the size of buffer
 *
 * @return the actually read size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
size_t device_read(device_t dev,
                         off_t    pos,
                         void       *buffer,
                         size_t   size)
{
    //ASSERT(dev != NULL);

    if (dev->ref_count == 0)
    {
      //  set_errno(-ERROR);
        return 0;
    }

    /* call device read interface */
    if (dev->read != NULL)
    {
        return dev->read(dev, pos, buffer, size);
    }

    /* set error code */
    //set_errno(-ENOSYS);

    return 0;
}


/**
 * This function will write some data to a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of written
 * @param buffer the data buffer to be written to device
 * @param size the size of buffer
 *
 * @return the actually written size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
size_t device_write(device_t dev,
                          off_t    pos,
                          const void *buffer,
                          size_t   size)
{
    //ASSERT(dev != NULL);

    if (dev->ref_count == 0)
    {
        //set_errno(-ERROR);
        return 0;
    }

    /* call device write interface */
    if (dev->write != NULL)
    {
        return dev->write(dev, pos, buffer, size);
    }

    /* set error code */
    //set_errno(-ENOSYS);

    return 0;
}

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param dev the pointer of device driver structure
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
err_t device_control(device_t dev, uint8_t cmd, void *arg)
{
 //   ASSERT(dev != NULL);

    /* call device write interface */
    if (dev->control != NULL)
    {
        return dev->control(dev, cmd, arg);
    }

    return EOK;
}



/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or NULL on failure.
 */
device_t device_find(const char *name)
{
    struct object *object;
    struct list_node *node;

    /* enter critical */
    //if (thread_self() != NULL)
    //    enter_critical();

    /* try to find device object */
    for (node  = device_list.next;
         node != &(device_list);
         node  = node->next)
    {
        object = list_entry(node, struct object, list);
        if (strcmp(object->name, name) == 0)
        {
            /* leave critical */
            //if (thread_self() != NULL)
            //    exit_critical();

            return (device_t)object;
        }
    }

    /* leave critical */
    //if (thread_self() != NULL)
    //    exit_critical();

    /* not found */
    return NULL;
}

/**
 * This function will initialize the specified device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
err_t device_init(device_t dev)
{
    err_t result = EOK;


    /* get device init handler */
    if (dev->init != NULL)
    {
        if (!(dev->flag & DEVICE_FLAG_ACTIVATED))
        {
            result = dev->init(dev);
            if (result != EOK)
            {
                printf("To initialize device:%s failed. The error code is %d\n",
                           dev->parent.name, result);
            }
            else
            {
                dev->flag |= DEVICE_FLAG_ACTIVATED;
            }
        }
    }

    return result;
}

/**
 * This function will Initialize the linked list
 *
 * @param  None
 *
 * @return None
 */
void device_listInit(void)
{
	list_init(&device_list);
}



