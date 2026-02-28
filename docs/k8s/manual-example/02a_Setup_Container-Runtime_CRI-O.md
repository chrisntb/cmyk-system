# K8s Cluster - Manual Example - Setup Container Runtime - CRI-O

> In order to setup the Container Runtime CRI-O, you must first have completed the Ad Hoc Cluster setup prerequisite tasks, see [Setup](02_Setup.md)

> After you have completed these instructions, return to where you left off in the [Setup](02_Setup.md#container-runtime---all-nodes) page

These instruction were adapted from the official installation instructions:

- `https://kubernetes.io/docs/setup/production-environment/container-runtimes/#cri-o`

In short the setup:

- Installs the container runtime
  - Includes high and low level container runtime
  - Includes container networking interface
- Installs a CLI for working with the container runtime
- Note proxy settings are inherited from those configured for Systemd

## On All Nodes

Create setup script: setup-cri-o.sh

```shell
cat <<'EOF' > setup-cri-o.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-cri-o.sh
#
# ********

# See https://github.com/cri-o/cri-o/releases
# -> Use #.# for packages from https://download.opensuse.org/repositories/isv:/cri-o
export CRIO_VERSION="1.34"

# See https://github.com/kubernetes-sigs/cri-tools/releases
export CRICTL_VERSION="1.35.0"

echo
echo "-------- SETTING UP --------"
echo "CRIO_VERSION=${CRIO_VERSION}"
echo "CRICTL_VERSION=${CRICTL_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Container runtime --------"
curl -fsSL https://download.opensuse.org/repositories/isv:/cri-o:/stable:/v${CRIO_VERSION}/deb/Release.key |
  gpg --dearmor -o /etc/apt/keyrings/cri-o-apt-keyring.gpg

echo "deb [signed-by=/etc/apt/keyrings/cri-o-apt-keyring.gpg] https://download.opensuse.org/repositories/isv:/cri-o:/stable:/v${CRIO_VERSION}/deb/ /" |
  tee /etc/apt/sources.list.d/cri-o.list

apt-get update -y

apt-get install -y cri-o
apt-mark hold cri-o

tee -a /etc/default/crio <<EOFF
http_proxy="${http_proxy}"
https_proxy="${https_proxy}"
no_proxy="${no_proxy}"
EOFF

systemctl start crio.service
systemctl status crio.service
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
chmod 700 setup-cri-o.sh

sudo ./setup-cri-o.sh 2>&1 | tee -a setup-cri-o.sh.log
```

Checks:

```shell
crio --version
# crio version 1.34.4
#    GitCommit:      cc8e8609ed352d8e34b12e8a659fe4dd3bbb7892
#    GitCommitDate:  2026-01-05T15:24:21Z
#    GitTreeState:   dirty
#    BuildDate:      1970-01-01T00:00:00Z
#    GoVersion:      go1.24.6
#    Compiler:       gc
#    Platform:       linux/amd64
#    Linkmode:       static
#    BuildTags:
#      static
#      netgo
#      osusergo
#      exclude_graphdriver_btrfs
#      seccomp
#      apparmor
#      selinux
#    LDFlags:          unknown
#    SeccompEnabled:   true
#    AppArmorEnabled:  false

systemctl status crio
# Expect a running crio.service

sudo cat /proc/$(systemctl show -p MainPID crio.service | cut -d= -f2)/environ | tr '\0' '\n' | grep proxy
# Expect correct HTTP proxy settings

crictl --version
# crictl version v1.35.0

sudo crictl info | grep CgroupManagerName
# -> "CgroupManagerName": "systemd",

# Check proxy settings work
sudo crictl pull docker.io/library/hello-world:latest
# Image is up to date for docker.io/library/hello-world@sha256:27...df
sudo crictl images
# IMAGE                           TAG                 IMAGE ID            SIZE
# docker.io/library/hello-world   latest              1b44b5a3e06a9       26.7kB
```
