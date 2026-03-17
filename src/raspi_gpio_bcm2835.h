
#ifndef raspi_gpio_bcm2835_h
#define raspi_gpio_bcm2835_h

#include <devicetree/devicetree.h>





class gpio_bcm2835 : public device
{
public:
    gpio_bcm2835( string devName, bool verbose, uint32_t type, bool checkStatus ) : device( devName, verbose, type, checkStatus ){ cout << "bcm2835 constructor" << endl; myRegisterCount = mySize/4;};
	static string getCompatibility(){ return "brcm,bcm2711-gpio"; };

    void regBackup();
    void regRestore();
    void setSettings();

private:




};
















#endif
