# xmake
apt install build-essential unzip
mkdir -p log/stream
wget https://xmake.io/shget.text -O - | bash
echo "source ~/.xmake/profile" >> ~/.bashrc
echo "export XMAKE_ROOT=y" >> ~/.bashrc
source ~/.bashrc