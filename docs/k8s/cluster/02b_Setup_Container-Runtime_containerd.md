# K8s Cluster Example - Setup Container Runtime - containerd

> In order to setup the Container Runtime containerd, you must first have completed the Ad Hoc Cluster setup prerequisite tasks, see [Setup](02_Setup.md)

> After you have completed these instructions, return to where you left off in the [Setup](02_Setup.md#container-runtime---all-nodes) page

These instruction were adapted from option 1 of the official installation instructions:

- `https://github.com/containerd/containerd/blob/main/docs/getting-started.md#option-1-from-the-official-binaries`

In short the setup:

- Installs the low level container runtime
  - Installs the container networking interface
  - Installs the high level container runtime
- Makes sure the container runtime
  - Uses the correct control group driver
- Installs a CLI for working with the container runtime
- Note proxy settings are inherited from those configured for Systemd

## On All Nodes

Create setup script: setup-containerd.sh

```shell
cat <<'EOF' > setup-containerd.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-containerd.sh
#
# ********

# See https://github.com/opencontainers/runc/releases
export RUNC_VERSION="1.4.0"

# See https://github.com/containernetworking/plugins/releases
export CNI_VERSION="1.9.0"

# See https://github.com/containerd/containerd/releases
export CONTAINERD_VERSION="2.2.1"

# See https://github.com/kubernetes-sigs/cri-tools/releases
export CRICTL_VERSION="1.35.0"

echo
echo "-------- SETTING UP --------"
echo "RUNC_VERSION=${RUNC_VERSION}"
echo "CNI_VERSION=${CNI_VERSION}"
echo "CONTAINERD_VERSION=${CONTAINERD_VERSION}"
echo "CRICTL_VERSION=${CRICTL_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Low level container runtime --------"
curl -fL -o "runc.amd64" "https://github.com/opencontainers/runc/releases/download/v${RUNC_VERSION}/runc.amd64"
install -m 755 runc.amd64 /usr/local/sbin/runc
echo "-"
echo

echo
echo "-------- SETTING UP - Container networking interface --------"
curl -fL -o "cni-plugins-linux-amd64-v${CNI_VERSION}.tgz" "https://github.com/containernetworking/plugins/releases/download/v${CNI_VERSION}/cni-plugins-linux-amd64-v${CNI_VERSION}.tgz"
mkdir -p /opt/cni/bin
tar Cxzvf /opt/cni/bin cni-plugins-linux-amd64-v${CNI_VERSION}.tgz
echo "-"
echo

echo
echo "-------- SETTING UP - Container runtime --------"
curl -fL -o "containerd-${CONTAINERD_VERSION}-linux-amd64.tar.gz" "https://github.com/containerd/containerd/releases/download/v${CONTAINERD_VERSION}/containerd-${CONTAINERD_VERSION}-linux-amd64.tar.gz"
tar Cxzvf /usr/local containerd-${CONTAINERD_VERSION}-linux-amd64.tar.gz

curl -fL -o "containerd.service" "https://raw.githubusercontent.com/containerd/containerd/main/containerd.service"
mkdir -p /usr/local/lib/systemd/system/
cp containerd.service /usr/local/lib/systemd/system/
systemctl daemon-reload
systemctl enable --now containerd

mkdir /etc/containerd/
containerd config default | tee /etc/containerd/config.toml > /dev/null
echo "-"
echo

echo
echo "-------- SETTING UP - Container runtime - Control group driver --------"
sed -i 's/^\([[:space:]]*\)SystemdCgroup\([[:space:]]*\)=\([[:space:]]*\)false/\1SystemdCgroup\2=\3true/' /etc/containerd/config.toml
echo "-"
echo

echo
echo "-------- SETTING UP - Restarting container runtime to pick up the new config --------"
systemctl daemon-reload
systemctl restart containerd
echo "-"
echo

echo
echo "-------- SETTING UP - Container runtime interface CLI --------"
curl -fL -o "crictl-v${CRICTL_VERSION}-linux-amd64.tar.gz" "https://github.com/kubernetes-sigs/cri-tools/releases/download/v${CRICTL_VERSION}/crictl-v${CRICTL_VERSION}-linux-amd64.tar.gz"
tar zxvf crictl-v${CRICTL_VERSION}-linux-amd64.tar.gz -C /usr/local/bin
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run setup script:

```shell
chmod 700 setup-containerd.sh

sudo ./setup-containerd.sh 2>&1 | tee -a setup-containerd.sh.log
```

Checks:

```shell
containerd --version
# containerd github.com/containerd/containerd/v2 v2.2.1 dea7da592f5d1d2b7755e3a161be07f43fad8f75

systemctl status containerd
# Expect a running containerd.service

grep -n 'SystemdCgroup' /etc/containerd/config.toml
# -> SystemdCgroup = true

sudo systemctl show containerd -p LoadState,DropInPaths
# LoadState=loaded
# DropInPaths=

sudo cat /proc/$(systemctl show -p MainPID containerd.service | cut -d= -f2)/environ | tr '\0' '\n' | grep proxy
# Expect correct HTTP proxy settings

crictl --version
# crictl version v1.35.0

sudo crictl info | grep SystemdCgroup
# -> "SystemdCgroup": true

# Check proxy settings work
sudo crictl pull docker.io/library/hello-world:latest
sudo crictl images
# IMAGE                           TAG                 IMAGE ID            SIZE
# docker.io/library/hello-world   latest              1b44b5a3e06a9       16.3kB
```
