#!/bin/bash

sudo /home/pi/root_scripts/bash_split > /dev/null &

echo -e "Content-type: text/html\n\n"

echo "<h1>Inkling Restarted</h1>"

exit 0


