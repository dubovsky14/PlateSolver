The package implements plate solving (i.e. calculating celestial coordinates from a photo of the night sky).
The code uses the algorithm published in 2009 by D. Lang, D. W. Hogg, K. Mierle, M. Blantom and S. Roweis: https://arxiv.org/pdf/0910.2233.pdf

In order to be able to plate solve, one has to have two files:
1) csv file with celestial coordinates, indices and magnitudes of the stars. This file can be produced using ```python/get_catalogue.py``` script - it will retrieve the data about the stars from SIMBAD database and dump them into csv file. You will need an internet access to be able to run the script.
2) File with asterism hashes (described in the paper from arXiv) and indices of the stars used in the hashes. One can use either text file, or binary (with ```.bin``` extension). Binary file is recommended, since it's faster to read. Text file can be used for better visualization, debugging and development purposes. In order to get the hash file, use ```bin/create_hash_file``` executable, it takes 3 input arguments - path to the csv file with stars information, focal length and address of the output hash file. The focal should approximately match the effective focal length of the photo that you would like to plate-solve using this file.

Once you have the two files described above, you can use the code to plate-solve your photo.
The code will identify stars in your photo, select few brightest stars of them, create asterism hashes for all combinations of these stars and compare the hashes with the pre-calculated hashes from the hash file.
It will find the most similar hashes and it will then go through all of them and verify if the stars from photo match the stars from the database around the hashed stars.

Installing the dependencies:
----------------------------
If you do not have the basic C++ development tools, you will need ```gcc``` and ```cmake```. You will also need ```python3-dev```.
In order to install these packages on Ubuntu:

```
sudo apt-get install python3-dev gcc cmake
```

Now you need to install OpenCV (```https://opencv.org/```) library, in order to do that on Ubuntu:

```sudo apt-get update -y || true &&  DEBIAN_FRONTEND=noninteractive sudo apt-get install -y tzdata libx11-dev && sudo apt-get install -y --no-install-recommends libopencv-dev```

for the python part of the package (GUI and some scripts) you need to install some python packages:

```pip3 install -r ../requirements.txt```

At this point, you should have all the dependencies installed. Now you can compile the code.

Checking out and compiling the code
-----------------------------------
```
    git clone git@github.com:dubovsky14/PlateSolver.git

    cd PlateSolver

    mkdir bin

    cd bin

    cmake ../.

    make # optionally use "make -j4" for compiling on 4 CPUs, or "make -j" to compile on all available CPUs

```


How to use the code:
----------------------

Now you will need to produce the csv file with the positions of the stars.
It will need ```hypatie``` module for python, so if you do not have it, you should install if first by ```pip3 install hypatie```.
Now you can use the provided python script to get the csv file with stars info:

```
python3 python/get_catalogue.py <address of the csv file you would like to produce>

```


Once you have the csv file with the stars positions, you can produce hash files. This process can take up to few hours, depending on the focal length:

```
./bin/create_hash_file <path to the csv file with stars info> <focal length in mm> <output file with the hashes - it's recommended to use extension .bin for this file - in this case it will use binary format for the file which is faster to read>
```

The focal length should approximately match effective focal length of the photos that you would like to plate solve.
Deviation up to 50% is usually fine, but larger deviations can lead to failures in the plate-solving.

Once you have the hash file and the csv file, you can plate solve your photo:

```
./bin/plate_solve <path to the csv file with stars info> <path to the hash file> <path to the jpg file you would like to plate-solve>
```

Python binding:
----------------

The ```python``` folder contains python wrapper for the C++ part - ```plate_solving_wrapper.py```.
The folder also contains the example python script ```wrapper_example.py``` on how to the wrapper.

GUI:
-----

The framework also contains a graphical user interface for running the tool as a web application. In order to use it:

```
cd gui

uwsgi --http-socket :9090 --wsgi-file run_localhost.py --master
```

Side note: in order to be able to use the GUI, your csv file with positions of the stars, and your hash files have to be in the ```data``` folder. The name of the star csv file must be ```catalogue.csv``` and your index files must have the name ```index_file_<something>.bin```, where ```<something>``` might be any string, for example focal length.


Running on Raspberry Pi:
------------------------

In order to run the app as system service on Rapsberry Pi do the following, but firstly open ```app.service``` and change the paths accordingly (also change port number if you would like to choose a different port).

```
cd gui

sudo cp app.service /etc/systemd/system/

sudo systemctl daemon-reload

sudo systemctl start app.service

sudo systemctl enable app.service

```

In order to be able to access the page from other devices on the network, you have to expose the port 9090:

```
sudo ufw allow 9090
```

If you want to hide it again:
```
fuser -k 9090/tcp
```

The code and tutorial might be useful if you want to run in on Raspberry Pi and create a wifi access point from it:

```https://github.com/oblique/create_ap```