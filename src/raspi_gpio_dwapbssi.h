#ifndef raspi_gpio_dwapbssi_h
#define raspi_gpio_dwapbssi_h

#include <devicetree/devicetree.h>




class gpio_dwapbssi : public device
{
public:
    gpio_dwapbssi( string devName, bool verbose, uint32_t type, bool checkStatus ) : device( devName, verbose, type, checkStatus ){ cout << "bcm2835 constructor" << endl; myRegisterCount = mySize/4;};
	static string getCompatibility(){ return "raspberrypi,rp1-gpio"; };

    void regBackup();
    void regRestore();
    void setSettings();

private:




};









#endif
