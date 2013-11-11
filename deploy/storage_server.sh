sudo apt-get update -q -y
sudo apt-get install nfs-kernel-server nfs-common -q -y
sudo mkdir /deploy
sudo sh -c 'echo "/deploy *(ro,nohide)" >> /etc/exports'
sudo service nfs-kernel-server restart
