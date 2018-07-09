/* Button Input Subsystem Driver */
/* Device has just one button it is accessible at i/o port BUTTON_PORT */
/* When pressed or released a BUTTON_IRQ happens */

#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>

#define BUTTON_IRQ_NO 17
#define BUTTON_PORT   10

static struct input_dev *button_dev;

/* Driver Private Structure */
/* Required if the IRQ is SHARED */
static struct dev_priv_obj_t
{
  int dummy;
}dev_priv_obj;

/* This is the function which executes continuously in the driver */
static irqreturn_t button_interrupt_handler(int irq, void *dev_private_obj)
{
  input_report_key(button_dev, BTN_0, inb(BUTTON_PORT) & 1);
  input_sync(button_dev);
  pr_info("In IRQ\r\n");
  return IRQ_HANDLED;
}

static int __init button_init(void)
{
  int retval;

  pr_info("Input Button Driver : %s Invoked\r\n", __func__);

  /* Allocate an Interrupt Line for Button */
  retval = request_irq(BUTTON_IRQ_NO, button_interrupt_handler, IRQF_SHARED, "button", &dev_priv_obj);
  if(retval)
  {
    pr_err("Failed to Allocate Button IRQ\r\n");
    return retval;
  }

  /* Allocate a new input device */
  button_dev = input_allocate_device();
  if(!button_dev)
  {
    pr_err("Failed to Allocate Input Device\r\n");
    free_irq(BUTTON_IRQ_NO, &dev_priv_obj);
    return -ENOMEM;
  }

  /* */
  button_dev->evbit[0] = BIT_MASK(EV_KEY);
  /* */
  button_dev->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0);

  /* Adds the button_dev structure to linked lists of the input driver and
     calls device handler modules _connect functions to tell them a new input
     device has appeared */
  retval = input_register_device(button_dev);
  if(retval)
  {
    pr_err("Failed to Register Device\r\n");
    /* Only called to free the memory occupied by button_dev incase the 
       input_register_device() call fails */
    input_free_device(button_dev);
    free_irq(BUTTON_IRQ_NO, &dev_priv_obj);
    return retval;
  }
  return 0;;
}

static void __exit button_exit(void)
{
  pr_info("Input Button Driver : %s Invoked\r\n", __func__);

  /* Release the allocated input_device object */
  input_unregister_device(button_dev);
  /* Release the IRQ */
  free_irq(BUTTON_IRQ_NO, &dev_priv_obj);
}

module_init(button_init);
module_exit(button_exit);

MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Driver to Read Button using Input Subsystem");
MODULE_LICENSE("GPL");

