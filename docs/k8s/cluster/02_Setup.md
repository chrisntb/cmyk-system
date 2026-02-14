# K8s Cluster Example - Setup

These instruction were adapted from the official installation instructions:

- `https://kubernetes.io/docs/setup/production-environment/`
  - `https://github.com/cri-o/packaging/blob/main/README.md#distributions-using-deb-packages`
  - `https://kubernetes.io/docs/tasks/administer-cluster/kubeadm/adding-linux-nodes/`

In short the setup:

- Determines the cluster networking
- On all nodes
  - Installs prerequisites
  - Installs a Container Runtime
    - [Setup Container Runtime - CRI-O](02a_Setup_Container-Runtime_CRI-O.md)
    - OR [Setup Container Runtime - containerd](02b_Setup_Container-Runtime_containerd.md)
  - Installs K8s services
- On controller node
  - Installs Helm
  - Initializes the cluster
  - Deploys a Container Network Interface add-on
- On worker nodes
  - Joins node to the cluster

After you have the cluster setup:

- Add support to the cluster for advanced job admission and placement logic, integrating with the native scheduler
  - [Setup Kueue](03_Setup_Kueue.md)
- Add support to the cluster for advanced job admission and placement logic, replacing the native scheduler for certain workloads
  - [Setup KAI Scheduler](04_Setup_KAI-Scheduler.md)
- Add support to the cluster for discovering, sharing and monitoring Nvidia GPUs
  - [Setup Nvidia GPU Operator](05_Setup_Nvidia-GPU-Operator.md)
- Run tests to validate the cluster is working e.g. run CPU and GPU jobs
  - [Tests](06_Tests.md)
- Add monitoring
  - [Setup Monitoring](07_Setup_Monitoring.md)

## Cluster Network

Decide on the IPs for the cluster:

```shell
POD_NETWORK_CIDR="10.244.0.0/16"  # K8s default
SERVICE_CIDR="10.96.0.0/12"       # K8s default
NODE_IPS="11.11.1.1,11.11.1.2,11.11.1.3"
```

## Prerequisites - All Nodes

All nodes that are part of the cluster must report themselves a unique and so if you are using virtual machines, it is good idea to check for this before starting:

```shell
# Check unique MAC
ip link

# Check unique product UUID
sudo cat /sys/class/dmi/id/product_uuid
```

Nodes need the following configuration:

- HTTP Proxy configuration
- OS patches applied
- Required tools installed
- Swap disabled
- IPv4 packet routing between interfaces
  - See `https://kubernetes.io/docs/setup/production-environment/container-runtimes/#prerequisite-ipv4-forwarding-optional`
- Packet filtering on network bridge devices
- Bridged pod network traffic must hit iptables chains
  - This will be managed by kube-proxy in order to maintain proper connection tracking for services and network policies

> Your NO_PROXY must include your POD_NETWORK_CIDR, SERVICE_CIDR, and NODE_IPS to avoid proxy loops on internal traffic

Create setup script: setup-node.sh

```shell
cat <<'EOF' > setup-node.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-node.sh HTTP_PROXY POD_NETWORK_CIDR SERVICE_CIDR NODE_IPS
#
# ********

HTTP_PROXY=""
if [[ -n $1 ]]; then
  HTTP_PROXY=$1
else
  echo "Parameter 'HTTP_PROXY' required"
  exit 2
fi

POD_NETWORK_CIDR=""
if [[ -n $2 ]]; then
  POD_NETWORK_CIDR=$2
else
  echo "Parameter 'POD_NETWORK_CIDR' required"
  exit 2
fi

SERVICE_CIDR=""
if [[ -n $3 ]]; then
  SERVICE_CIDR=$3
else
  echo "Parameter 'SERVICE_CIDR' required"
  exit 2
fi

NODE_IPS=""
if [[ -n $4 ]]; then
  NODE_IPS=$4
else
  echo "Parameter 'NODE_IPS' required"
  exit 2
fi

NO_PROXY="localhost,127.0.0.1,${POD_NETWORK_CIDR},${SERVICE_CIDR},${NODE_IPS},.svc,.cluster.local"

echo
echo "-------- SETTING UP --------"
echo "HTTP_PROXY=${HTTP_PROXY}"
echo "POD_NETWORK_CIDR=${POD_NETWORK_CIDR}"
echo "SERVICE_CIDR=${SERVICE_CIDR}"
echo "NODE_IPS=${NODE_IPS}"
echo "NO_PROXY=${NO_PROXY}"
echo "-"
echo

echo
echo "-------- SETTING UP - Configure HTTP proxy --------"
echo "Add the proxies to '/etc/environment'"
cat <<EOFF | tee -a /etc/environment

http_proxy="${HTTP_PROXY}"
https_proxy="${HTTP_PROXY}"
no_proxy="${NO_PROXY}"
EOFF

echo "Add the proxies to '/etc/systemd/system.conf.d/99-proxy.conf'"
mkdir -p /etc/systemd/system.conf.d
cat <<EOFFF | tee /etc/systemd/system.conf.d/99-proxy.conf
[Manager]
DefaultEnvironment="http_proxy=${HTTP_PROXY}"
DefaultEnvironment="https_proxy=${HTTP_PROXY}"
DefaultEnvironment="no_proxy=${NO_PROXY}"
EOFFF
sudo systemctl daemon-reexec
echo "-"
echo

echo
echo "-------- SETTING UP - Apply updates --------"
apt-get update -y
apt-get upgrade -y
echo "-"
echo

echo
echo "-------- SETTING UP - Install packages --------"
echo "Installing required packages"
apt-get install -y apt-transport-https ca-certificates gpg software-properties-common curl

echo "Installing useful packages"
apt-get install -y dnsutils net-tools netcat-traditional rsync socat
echo "-"
echo

echo
echo "-------- SETTING UP - Disable swap --------"
sed -i '/[[:space:]]swap[[:space:]]/s/^/#/' /etc/fstab
cat /etc/fstab
echo "-"
echo

echo
echo "-------- SETTING UP - IPv4 packet routing --------"
# Ensure loaded a boot
cat <<EOFFF | tee /etc/sysctl.d/k8s.conf
net.ipv4.ip_forward = 1
EOFFF
echo "-"
echo

echo
echo "-------- SETTING UP - Packet filtering on network bridge devices --------"
modprobe br_netfilter

# Ensure loaded a boot
echo 'br_netfilter' | tee -a /etc/modules-load.d/k8s.conf
echo "-"
echo

echo
echo "-------- SETTING UP - Bridged traffic for iptables chains --------"
# Ensure loaded a boot
cat <<EOFFFF | tee -a /etc/sysctl.d/k8s.conf
net.bridge.bridge-nf-call-iptables = 1
net.bridge.bridge-nf-call-ip6tables = 1
EOFFFF
echo "-"
echo

echo
echo "-------- SETTING UP - Apply sysctl params without reboot --------"
sysctl --system
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run setup script:

```shell
chmod 700 setup-node.sh

HTTP_PROXY="http://###.###.###.###:####"

POD_NETWORK_CIDR="10.244.0.0/16"
SERVICE_CIDR="10.96.0.0/12"
NODE_IPS="11.11.1.1,11.11.1.2,11.11.1.3"

sudo ./setup-node.sh ${HTTP_PROXY} ${POD_NETWORK_CIDR} ${SERVICE_CIDR} ${NODE_IPS} 2>&1 | tee -a setup-node.sh.log

sudo reboot
```

Checks after reboot:

```shell
# Check swap is off
swapon --show

# Check IPv4 packets can be routed between interfaces
sysctl net.ipv4.ip_forward
# -> 1
sysctl net.bridge.bridge-nf-call-iptables
# -> 1
sysctl net.bridge.bridge-nf-call-ip6tables
# -> 1

# Check proxy settings - environment
cat /etc/environment
echo ${http_proxy}
echo ${https_proxy}
echo ${no_proxy}

# Check proxy settings - systemd
cat /etc/systemd/system.conf.d/99-proxy.conf
systemctl show --property=Environment
```

## Container Runtime - All Nodes

All nodes must have a container runtime installed.
The two options we have tested are CRI-O and containerd, both with the low-level container runtime Runc.
Return to this page when, on all nodes, you have completed the setup tasks described in one of these pages:

- `CRI-O`
  - [Setup Container Runtime - CRI-O](02a_Setup_Container-Runtime_CRI-O.md)
- `containerd`
  - [Setup Container Runtime - containerd](02b_Setup_Container-Runtime_containerd.md)

## Kubernetes - Installation - All Nodes

With a container runtime now available, install K8s on all nodes.

Create setup script: setup-k8s.sh

```shell
cat <<'EOF' > setup-k8s.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-k8s.sh
#
# ********

# See https://github.com/kubernetes/kubernetes/releases
export K8S_VERSION="1.35"

echo
echo "-------- SETTING UP --------"
echo "K8S_VERSION=${K8S_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - K8s --------"
curl -fsSL https://pkgs.k8s.io/core:/stable:/v${K8S_VERSION}/deb/Release.key |
    sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg

echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v${K8S_VERSION}/deb/ /" |
  tee /etc/apt/sources.list.d/kubernetes.list

apt-get update -y

# kubectl is not required on workers but is sometimes installed for convenience
# -> When debugging on a worker with kubectl,
# ->   after running 'sudo kubeadm init --pod-network-cidr=${K8S_CLUSTER_IP_CIDR}' on the controller,
# ->   you need to copy '/etc/kubernetes/admin.conf' from the controller node and run these commands:
# ->-> mkdir -p $HOME/.kube
# ->-> sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
# ->-> sudo chown $(id -u):$(id -g) $HOME/.kube/config
apt-get install -y kubelet kubeadm kubectl
apt-mark hold kubelet kubeadm kubectl
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run the setup script:

```shell
chmod 700 setup-k8s.sh

sudo ./setup-k8s.sh 2>&1 | tee -a setup-k8s.sh.log
```

Checks:

```shell
systemctl status kubelet
# Expect a dead crio.service

kubelet --version
# Kubernetes v1.35.0

kubeadm version
# kubeadm version: &version.Info{Major:"1", Minor:"35", EmulationMajor:"", EmulationMinor:"", MinCompatibilityMajor:"", MinCompatibilityMinor:"", GitVersion:"v1.35.0", GitCommit:"66452049f3d692768c39c797b21b793dce80314e", GitTreeState:"clean", BuildDate:"2025-12-17T12:39:26Z", GoVersion:"go1.25.5", Compiler:"gc", Platform:"linux/amd64"}

kubectl version
# Client Version: v1.35.0
# Kustomize Version: v5.7.1
# The connection to the server localhost:8080 was refused - did you specify the right host or port?

kubectl config view
# apiVersion: v1
# clusters: null
# contexts: null
# current-context: ""
# kind: Config
# users: null
```

## K8s - Installation - Helm - Controller Node

With K8s now available, install Helm on the controller node.

Create setup script: setup-helm.sh

```shell
cat <<'EOF' > setup-helm.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-helm.sh
#
# ********

# See https://github.com/helm/helm/releases
export HELM_VERSION="4"

echo
echo "-------- SETTING UP --------"
echo "HELM_VERSION=${HELM_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Helm --------"
curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/master/scripts/get-helm-${HELM_VERSION} \
  && chmod 700 get_helm.sh \
  && ./get_helm.sh
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run the setup script:

```shell
chmod 700 setup-helm.sh

sudo ./setup-helm.sh 2>&1 | tee -a setup-helm.sh.log
```

Checks:

```shell
helm version
# version.BuildInfo{Version:"v4.0.4", GitCommit:"8650e1dad9e6ae38b41f60b712af9218a0d8cc11", GitTreeState:"clean", GoVersion:"go1.25.5", KubeClientVersion:"v1.34"}
```

## Kubernetes - Initialization - Controller Node

Initialize k8s controller with the default control group driver `systemd`:

```shell
# IMPORTANT - These networks are those you have decided upon and configured in the various HTTP proxy settings above

export POD_NETWORK_CIDR="10.244.0.0/16"
export SERVICE_CIDR="10.96.0.0/12"

# Using the default control group driver `systemd`
# -> IMPORTANT - This command will log a command that you will later run on all worker nodes to join them to the cluster
sudo kubeadm init --pod-network-cidr=${POD_NETWORK_CIDR} --service-cidr=${SERVICE_CIDR}

# The 'ubuntu' user will be operating the cluster
mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

Checks:

```shell
systemctl status kubelet
# Expect a running crio.service

sudo cat /proc/$(systemctl show -p MainPID kubelet.service | cut -d= -f2)/environ | tr '\0' '\n' | grep proxy
# Expect correct HTTP proxy settings

grep -n 'cgroupDriver' /var/lib/kubelet/config.yaml
# -> 15:cgroupDriver: systemd

kubectl config view
# apiVersion: v1
# clusters:
# - cluster:
#     certificate-authority-data: DATA+OMITTED
#     server: https://11.11.1.1:6443
#   name: kubernetes
# contexts:
# - context:
#     cluster: kubernetes
#     user: kubernetes-admin
#   name: kubernetes-admin@kubernetes
# current-context: kubernetes-admin@kubernetes
# kind: Config
# users:
# - name: kubernetes-admin
#   user:
#     client-certificate-data: DATA+OMITTED
#     client-key-data: DATA+OMITTED
```

Output:

```shell
# [init] Using Kubernetes version: v1.35.0
# [preflight] Running pre-flight checks
#  [WARNING HTTPProxyCIDR]: connection to "10.96.0.0/12" uses proxy "http://###.###.###.###:####". This may lead to malfunctional cluster setup. Make sure that Pod and Services IP ranges specified correctly as exceptions in proxy configuration
# [preflight] Pulling images required for setting up a Kubernetes cluster
# ...
# [kubelet-finalize] Updating "/etc/kubernetes/kubelet.conf" to point to a rotatable kubelet client certificate and key
# [addons] Applied essential addon: CoreDNS
# [addons] Applied essential addon: kube-proxy

# Your Kubernetes control-plane has initialized successfully!

# To start using your cluster, you need to run the following as a regular user:

#   mkdir -p $HOME/.kube
#   sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
#   sudo chown $(id -u):$(id -g) $HOME/.kube/config

# Alternatively, if you are the root user, you can run:

#   export KUBECONFIG=/etc/kubernetes/admin.conf

# You should now deploy a pod network to the cluster.
# Run "kubectl apply -f [podnetwork].yaml" with one of the options listed at:
#   https://kubernetes.io/docs/concepts/cluster-administration/addons/

# Then you can join any number of worker nodes by running the following on each as root:

# kubeadm join 11.11.1.1:6443 --token 38...mh \
#  --discovery-token-ca-cert-hash sha256:ab...fb
```

## K8s - Initialization - Networking - Controller Node

On the controller node, deploy a Container Network Interface (CNI) network add-on so that your pods can communicate with each other.
Cluster DNS (CoreDNS) will not start up before a CNI network implementation is installed.
See `https://kubernetes.io/docs/concepts/cluster-administration/addons/`.
Note this is only installed on the controller.
The controller will schedule the pods automatically on all nodes.

We are using the Flannel CNI add-on, see `https://github.com/flannel-io/flannel/releases`.

Create the setup script: setup-cni-flannel.sh:

```shell
cat <<'EOF' > setup-cni-flannel.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-cni-flannel.sh
#
# ********

# See https://github.com/flannel-io/flannel/releases
export FLANNEL_VERSION="latest"

echo
echo "-------- SETTING UP --------"
echo "FLANNEL_VERSION=${FLANNEL_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Packages --------"
curl -fL -o kube-flannel.yml https://github.com/flannel-io/flannel/releases/latest/download/kube-flannel.yml
kubectl apply -f kube-flannel.yml
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run the setup script:

```shell
chmod 700 setup-cni-flannel.sh

# Note running as the 'ubuntu' user
./setup-cni-flannel.sh 2>&1 | tee -a setup-cni-flannel.sh.log
```

Checks:

```shell
kubectl get nodes -o wide

kubectl get pods --all-namespaces -o wide
kubectl logs -n kube-flannel kube-flannel-ds-...
```

## Kubernetes - Initialization - Worker Nodes

Now that the controller is initialized, worker nodes can join the cluster.
When the cluster was initialized on the controller node, the initialization command logged a command which when run on a worker node, allows it to join the cluster e.g.

```shell
sudo kubeadm join 11.11.1.1:6443 --token 38...mh \
  --discovery-token-ca-cert-hash sha256:ab...fb
```

Alternatively you can get the cluster `join` command at anytime by running this on the controller:

```shell
kubeadm token create --print-join-command
# kubeadm join 11.11.1.1:6443 --token 38...mh --discovery-token-ca-cert-hash sha256:ab...fb
```

Output on worker node:

```shell
kubeadm join 11.11.1.1:6443 --token 38...mh --discovery-token-ca-cert-hash sha256:ab...fb
# [preflight] Running pre-flight checks
# [preflight] Reading configuration from the "kubeadm-config" ConfigMap in namespace "kube-system"...
# [preflight] Use 'kubeadm init phase upload-config kubeadm --config your-config-file' to re-upload it.
# [kubelet-start] Writing kubelet configuration to file "/var/lib/kubelet/instance-config.yaml"
# [patches] Applied patch of type "application/strategic-merge-patch+json" to target "kubeletconfiguration"
# [kubelet-start] Writing kubelet configuration to file "/var/lib/kubelet/config.yaml"
# [kubelet-start] Writing kubelet environment file with flags to file "/var/lib/kubelet/kubeadm-flags.env"
# [kubelet-start] Starting the kubelet
# [kubelet-check] Waiting for a healthy kubelet at http://127.0.0.1:10248/healthz. This can take up to 4m0s
# [kubelet-check] The kubelet is healthy after 501.105949ms
# [kubelet-start] Waiting for the kubelet to perform the TLS Bootstrap

# This node has joined the cluster:
# * Certificate signing request was sent to apiserver and a response was received.
# * The Kubelet was informed of the new secure connection details.

# Run 'kubectl get nodes' on the control-plane to see this node join the cluster.
```

Checks on worker node:

```shell
systemctl status kubelet
# Expect a running crio.service

sudo cat /proc/$(systemctl show -p MainPID kubelet.service | cut -d= -f2)/environ | tr '\0' '\n' | grep proxy
# Expect correct HTTP proxy settings

grep -n 'cgroupDriver' /var/lib/kubelet/config.yaml
# -> 15:cgroupDriver: systemd

kubectl config view
# apiVersion: v1
# clusters: null
# contexts: null
# current-context: ""
# kind: Config
# users: null
```

Checks on controller node:

```shell
kubectl get nodes -o wide
# NAME   STATUS   ROLES           AGE     VERSION   INTERNAL-IP     EXTERNAL-IP   OS-IMAGE             KERNEL-VERSION     CONTAINER-RUNTIME
# wrk2   Ready    <none>          2m35s   v1.35.0   11.11.1.3       <none>        Ubuntu 24.04.3 LTS   6.8.0-90-generic   cri-o://1.34.3
# ctl    Ready    control-plane   7m17s   v1.35.0   11.11.1.1       <none>        Ubuntu 24.04.3 LTS   6.8.0-90-generic   cri-o://1.34.3
# wrk1   Ready    <none>          2m39s   v1.35.0   11.11.1.2       <none>        Ubuntu 24.04.3 LTS   6.8.0-90-generic   cri-o://1.34.3
```

## Common Errors

- Proxies :)

- Make sure you setup the required prerequisites for bridge networking, see above

- On the controller, when running `kubectl get nodes -o wide`, if you receive `couldn't get current server API group list: Get \"http://localhost:8080/api?timeout=32s\": dial tcp [::1]:8080: connect: connection refused`
  - You have not completed the instructions above, check your `$HOME/.kube/config` and the network add-on instructions
