#git clone https://github.com/marehr/intel-sde-downloader
#cd intel-sde-downloader
#pip install -r requirements.txt
#python ./intel-sde-downloader.py sde-external-8.35.0-2019-03-11-lin.tar.bz2
wget http://software.intel.com/content/dam/develop/external/us/en/protected/sde-external-8.50.0-2020-03-26-lin.tar.bz2
tar xvf sde-external-8.50.0-2020-03-26-lin.tar.bz2
sudo sh -c "echo 0 > /proc/sys/kernel/yama/ptrace_scope"
