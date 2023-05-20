# xmake
apt install build-essential unzip
wget https://xmake.io/shget.text -O - | bash
echo "source ~/.xmake/profile" >> ~/.bashrc
echo "export XMAKE_ROOT=y" >> ~/.bashrc
source ~/.bashrc