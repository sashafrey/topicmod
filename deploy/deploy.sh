cd ~/topicmod

sudo apt-get update -q -y
sudo apt-get install libcurl4-gnutls-dev libexpat1-dev gettext -q -y
sudo apt-get libz-dev libssl-dev -q -y
sudo apt-get install git -q -y
sudo apt-get install build-essential -q -y
sudo apt-get install libboost-all-dev -q -y
sudo apt-get install openjdk-6-jre -q -y

# ===== Download vowpal_wabbit library for topic modeling =====
git clone git://github.com/JohnLangford/vowpal_wabbit.git

## ==== configure git =====
git config --global user.name "Alexander Frey"
git config --global user.email "sashafrey@gmail.com"

## ===== unpack datasets =====
cd datasets
zcat docword.nips.txt.gz >docword.nips.txt
zcat docword.kos.txt.gz >docword.kos.txt
cd ..

## ===== Install Intel Composer and MKL =====
mkdir /opt/intel
mkdir /opt/intel/licenses
mv l_ccompxe_online_2013_sp1.1.106.license.lic /opt/intel/licenses/l_ccompxe_online_2013_sp1.1.106.license.lic
chmod 777 l_ccompxe_online_2013_sp1.1.106.sh
sudo ./l_ccompxe_online_2013_sp1.1.106.sh --silent l_ccompxe_online_2013_sp1.1.106.config.ini

