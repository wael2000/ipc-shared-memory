sudo pcs host auth rhel-ha-01 rhel-ha-02 -u hacluster
jzy2-4fm2-6ajn

10.0.2.2  rhel-ha-01
<IP-of-rhel-ha-02>  rhel-ha-02




sudo pcs host auth rhel-ha-01 rhel-ha-02 -u hacluster
pcs cluster setup mycluster rhel-ha-01 rhel-ha-02
pcs cluster setup mycluster 192.168.75.220 192.168.75.221
pcs cluster start --all
pcs cluster enable --all


# filewall 
sudo yum install firewalld -y
sudo systemctl enable firewalld
sudo systemctl start firewalld
sudo firewall-cmd --permanent --add-service=high-availability
sudo firewall-cmd --reload

#nc cli
sudo yum install nmap-ncat -y
nc -zv rhel-ha-01 2224

# change hacluster password
sudo passwd hacluster
jzy2-4fm2-6ajn



192.168.75.220  rhel-ha-01
192.168.75.221  rhel-ha-02





# 1. Create the user with the GECOS field
sudo useradd -c "HA Cluster User" -G users hacluster

# 2. Set the password
echo "hacluster:jzy2-4fm2-6ajn" | sudo chpasswd

# 3. Unlock the password (if it was locked)
sudo passwd -u hacluster

# 4. Give the user passwordless sudo
echo "hacluster ALL=(ALL) NOPASSWD:ALL" | sudo tee /etc/sudoers.d/hacluster
sudo chmod 440 /etc/sudoers.d/hacluster
