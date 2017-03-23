# WCCOAozw
OpenZWave driver for WinCC OA

The objective of this project is to provide a driver for WinCC OA that integrates ZWave devices supported by OpenZWave library (www.openzwave.com), notably on Raspberry Pi.

### Installation/Setup of WinCC OA on Raspberry PI

* You firstly need to obtain WinCC OA distribution for Raspberry Pi (from ETM). In what follows we will assume version 3.15
* make the installer files executable `chmod +x ./install ./install_api`
* install the necessary packages as suggested by the Readme.txt file
* execute `./install` as root, and accept to have it installed into the default location (/opt)
* execute `./install_api` as root; we are going to perform the compilation directly on the RPi, hence when prompted "WinCC OA Crosscompile API: (absolute path)", specify `/opt/WinCC_OA/3.15` ; this will create a subfolder "api" in the WinCC OA 3.15 installation that you've done above
* in addition, you will need cmake and g++: `apt-get install cmake gcc g++`
* subsequent steps don't require root privileges
* set the environment variables required for the WinCC OA API: 
 ```shell
   export API_ROOT=/opt/WinCC_OA/3.15/api
   export SYSROOT=/
   export TOOLCHAIN=/usr
 ```
   these specify that we would use the default compilers installed under /usr, and default includes and libs
 * verify that the whole build infrastructure works by instantiating a demo driver of WinCC OA; we will call it TestDriver;
   * go to the folder where you want to have the test source code generated, eg. `cd ~/work`
   * `/opt/WinCC_OA/3.15/api/newWCCILDriver.sh TestDriver`
   * this will generate a folder `TestDriver` with demo code and then start cmake to generate makefiles
   * `cd build; make` should do the succesful build
 * you may then create a new WinCC OA project, and check if the Driver which you build could be started (out of scope of this Readme) 
