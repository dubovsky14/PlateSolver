FROM ubuntu:22.04
WORKDIR ./PlateSolverDir
ADD gui gui
ADD python python
ADD requirements.txt requirements.txt

ADD src src
ADD PlateSolver PlateSolver
ADD utils utils
ADD CMakeLists.txt CMakeLists.txt

#index files and star catalogue
ADD data/catalogue.csv  data/catalogue.csv
ADD data/index_file_200mm.bin  data/index_file_200mm.bin
ADD data/index_file_300mm.bin  data/index_file_300mm.bin
ADD data/index_file_400mm.bin  data/index_file_400mm.bin
ADD data/index_file_600mm.bin  data/index_file_600mm.bin
ADD data/index_file_840mm.bin  data/index_file_840mm.bin
ADD data/index_file_1000mm.bin  data/index_file_1000mm.bin
ADD data/index_file_1200mm.bin  data/index_file_1200mm.bin
ADD data/index_file_1400mm.bin  data/index_file_1400mm.bin


WORKDIR ./bin

RUN apt-get update
RUN apt-get install -y python3-dev python3-pip
RUN apt-get install -y gcc cmake

RUN apt-get update -y || true && \
DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata libx11-dev && \
apt-get install -y --no-install-recommends libopencv-dev && \
     rm -rf /var/lib/apt/lists/* && apt autoremove && apt clean

RUN cmake ../.
RUN make -j4

WORKDIR ./../gui

RUN pip3 install --upgrade pip
RUN pip3 install -r ../requirements.txt
#
#"uwsgi --http-socket 9090 --wsgi-file run_localhost.py --master
CMD ["uwsgi","--http-socket",":9090","--wsgi-file","run_localhost.py","--master"]
EXPOSE 9090
