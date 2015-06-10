#Experiment Log

##~~Start a new noob setup~~
~~https://www.raspberrypi.org/help/noobs-setup/~~

##Start a RASPBIAN setup

sudo raspi-config

##Install Node JS
~~sudo apt-get install nodejs~~

~~sudo apt-get install npm~~

wget http://node-arm.herokuapp.com/node_latest_armhf.deb 

sudo dpkg -i node_latest_armhf.deb

npm install ws       ~~_(Do this in code path)_~~

npm install express

##Install .local domain (seems already installed on new release)
~~sudo apt-get install avahi-daemon~~ Not needed for new release

##Compile C code in inklingcode
Follow how_I_compile.txt, ~~compiled code need to be run with sudo~~.

Copy _inkling.rules_ to _/etc/udev/rules.d/41-inkling.rules_ and run _sudo service udev restart_

Then you don't need root to access inkling.

##Use nodejs on 80 port
sudo iptables -t nat -A PREROUTING -i eth0 -p tcp --dport 80 -j REDIRECT --to-port 8080
 
##Run node in background
nohup node main_inkling.js &

