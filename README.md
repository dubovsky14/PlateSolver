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


Checking out and compiling the code
-----------------------------------

```
    git clone git@github.com:dubovsky14/PlateSolver.git

    cd PlateSolver

    mkdir bin

    cd bin

    cmake ../.

    make # optionally use "make -j4" for compiling on 4 CPUs, or "make -j" to compile on all available CPUs

    cd ..

```

Now install the dependencies for the python part of the code:

```pip3 install -r requirements.txt```

At this point, you should have everything installed and ready to use.


How to prepare the necessary input files:
-----------------------------------------

Now you will need to produce the csv file with the positions of the stars.
The following command will produce that file, downloading stars from SIMBAD database (internet connection is needed to run the script).

```
python3 python/get_catalogue.py <address of the csv file you would like to produce>

```


Once you have the csv file with the stars positions, you can produce hash files. This process can take up to few hours, depending on the focal length:

```
./bin/create_hash_file <path to the csv file with stars info> <focal length in mm> <output file with the hashes>
```

The focal length should approximately match effective focal length of the photos that you would like to plate solve.
Deviation up to 50% is usually fine, but larger deviations can lead to failures in the plate-solving.

Output file can be one of these three types:

1) text file (extension ```.txt``` or ```.csv```). This is slowest to read, but you can open it in any text editor and check the hashes. Useful for visualization/debugging. Each line consists of 8 numbers: hash composed of 4 floats followed by indices (unsigned ints) of stars A, B, C and D.

2) binary file (extension ```.bin``` ) - similar structure to the text file, but the file already is in binary form, so it's smaller and faster to read.

3) kd-tree (extension ```.kdtree```) - improved binary file - it's larger, but contains additional indices allowing the code to perform kd-tree nearest neighbors search. Hash extraction is more than 100 times faster compared to normal binary or text file, but the file is 50% larger in size.


If the space is not an issue (usually it's not), it's recommended to use ```.kdtree``` extension, since it allows for significantly faster searches.


How to plate-solve your photo using terminal:
---------------------------------------------

Once you have the hash file and the csv file, you can plate solve your photo using the executable ```bin/plate_solve``` through terminal:

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

This code and tutorial might be useful if you want to run in on Raspberry Pi and create a wifi access point from it: https://github.com/oblique/create_ap . However, you will have to modify the config file when you want to run it as system service. The example config file can be found in this (PlateSolver) repository: ```cat gui/create_ap/create_ap.service```