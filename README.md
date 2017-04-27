# GSm-based-automatic-notice-board
This project will let the admin update the notice board using the sms.

It is based on Atmega328P which is known in arduino with SIM800H gdm module.
During prototype of this, project, am using the 16X4 LCD for displaying the message in scrolling mode.

The prototype has two mode:
1. Automatic mode:
   The sysetm knows that it has to display all message in memory automaticallly, that`s mean that, it will display the message one after another.
 
 2. Manual Mode:
    This mode let the use to select which message he/she want to see using push buttons.
    
    Am using a Switch to switch modes.
    
    Here is the GSm module used: https://www.olimex.com/Products/Duino/AVR/OLIMEXINO-NANO-GSM/ 
