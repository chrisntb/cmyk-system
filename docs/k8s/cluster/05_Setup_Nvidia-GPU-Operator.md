# K8s Cluster Example - Setup Nvidia GPU Operator

> In order to setup the Nvidia GPU Operator you must first have completed the Ad Hoc Cluster setup tasks, see [Setup](02_Setup.md)

These instruction were adapted from the official installation instructions:

- `https://docs.nvidia.com/datacenter/cloud-native/gpu-operator/latest/getting-started.html`

In short the setup installs the Nvidia GPU Operator using its Helm chart.

## On Controller Node

Create setup script: setup-nvidia-gpu-operator.sh

```shell
cat <<'EOF' > setup-nvidia-gpu-operator.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-nvidia-gpu-operator.sh
#
# ********

# See https://github.com/NVIDIA/gpu-operator/releases
export NVIDIA_GPU_OPERATOR_VERSION="25.10.1"

echo
echo "-------- SETTING UP --------"
echo "NVIDIA_GPU_OPERATOR_VERSION=${NVIDIA_GPU_OPERATOR_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Helm Chart --------"
helm repo add nvidia https://helm.ngc.nvidia.com/nvidia \
  && helm repo update
echo "-"
echo

echo
echo "-------- SETTING UP - Values --------"
cat <<EOFF | tee nvidia-gpu-operator-values.yaml
driver:
  env:
    - name: http_proxy
      value: ${http_proxy}
    - name: https_proxy
      value: ${https_proxy}
    - name: no_proxy
      value: ${no_proxy}
EOFF
echo "-"
echo

echo
echo "-------- SETTING UP - Install --------"
helm install nvidia-gpu-operator nvidia/gpu-operator --namespace gpu-operator --create-namespace --version=v${NVIDIA_GPU_OPERATOR_VERSION} -f nvidia-gpu-operator-values.yaml
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run setup script:

```shell
chmod 700 setup-nvidia-gpu-operator.sh

# Note assumes proxy environment variables are available
# -> See /etc/environment

# Note running as the 'ubuntu' user
./setup-nvidia-gpu-operator.sh 2>&1 | tee -a setup-nvidia-gpu-operator.sh.log
```

Output:

```shell
I1231 21:19:21.038237   12773 warnings.go:110] "Warning: spec.template.spec.affinity.nodeAffinity.preferredDuringSchedulingIgnoredDuringExecution[0].preference.matchExpressions[0].key: node-role.kubernetes.io/master is use \"node-role.kubernetes.io/control-plane\" instead"
NAME: nvidia-gpu-operator
LAST DEPLOYED: Wed Dec 31 21:19:19 2025
NAMESPACE: gpu-operator
STATUS: deployed
REVISION: 1
DESCRIPTION: Install complete
TEST SUITE: None
```

Checks:

```shell
helm search repo nvidia
# NAME                                CHART VERSION APP VERSION DESCRIPTION
# nvidia/nvidia-device-plugin         0.9.0         0.9.0       A Helm chart for the nvidia-device-plugin on Ku...
# nvidia/nvidia-dra-driver-gpu        25.8.1        25.8.1      Official Helm chart for the NVIDIA DRA Driver f...
# nvidia/cybersecurity-dfp            0.2.1         23.07       Helm chart to deploy the Morpheus Digital Finge...
# nvidia/cybersecurity-sp             0.1.0         23.07       Helm chart to deploy the Morpheus Spear Phishin...
# nvidia/deepstream-its               0.2.0         1.0         A Helm chart for Kubernetes
# nvidia/dps                          0.7.5         0.7.5       NVIDIA Datacenter Power Service
# nvidia/dps-bmc-simulator            0.7.5         0.7.5       DPS BMC Simulator
# nvidia/ds-face-mask-detection       1.0.0         1.2         A Helm chart for Deepstream Intelligent Video A...
# nvidia/ds-lipactivity               0.0.1         0.0.1       DS Lip activity
# nvidia/fed-svr-3                    0.9.0         1.0         Federated Learning HELM Chart
# nvidia/fed-wrk-3                    0.9.0         1.0         Federated learning worker HELM Chart
# nvidia/gpu-operator                 v25.10.1      v25.10.1    NVIDIA GPU Operator creates/configures/manages ...
# nvidia/isaac-lab-teleop             2.2.0         0.0.0       A Helm chart for NVIDIA Isaac Lab Teleop
# nvidia/k8s-nim-operator             3.0.2         3.0.2       NVIDIA NIM Operator creates/configures/manages ...
# nvidia/network-operator             25.10.0       v25.10.0    Nvidia network operator
# nvidia/nspect_test_policy_org_chart 1             1.16.0      A Helm chart for Kubernetes
# nvidia/nvsm                         1.0.1         1.0.1       A Helm chart for deploying Nvidia System Manage...
# nvidia/tensorrt-inference-server    1.0.0         1.0         TensorRT Inference Server
# nvidia/tensorrtinferenceserver      1.0.0         1.0         Triton Inference Server Helm Chart
# nvidia/tritoninferenceserver_aws    0.1.0         1.16.0      A Helm chart for Kubernetes
# nvidia/video-analytics-demo         0.1.9         1.2         A Helm chart for Deepstream Intelligent Video A...
# nvidia/video-analytics-demo-l4t     0.1.3         0.1.3       Deepstream Intelligent Video Analytics Helm Cha...

helm list -A
# NAME                NAMESPACE     REVISION UPDATED                                  STATUS    CHART                 APP VERSION
# kai-scheduler       kai-scheduler 1        2026-01-09 03:29:28.798776451 +0000 UTC  deployed  kai-scheduler-v0.12.4 v0.12.4
# nvidia-gpu-operator gpu-operator  1        2025-12-31 21:19:19.875126086 +0000 UTC  deployed  gpu-operator-v25.10.1 v25.10.1

helm get values nvidia-gpu-operator -n gpu-operator -o yaml
# driver:
#   env:
#   - name: http_proxy
#     value: http://###.###.###.###:####
#   - name: https_proxy
#     value: http://###.###.###.###:####
#   - name: no_proxy
#     value: localhost,127.0.0.1,10.244.0.0/16,10.96.0.0/12,11.11.1.1,11.11.11.2,11.11.11.3,.svc,.cluster.local

kubectl get pods --all-namespaces -o wide
# NAMESPACE       NAME                                                              READY   STATUS     RESTARTS   AGE     IP              NODE        NOMINATED NODE   READINESS GATES
# gpu-operator    gpu-feature-discovery-c64lx                                       0/1     Init:0/1   0          2m3s    <none>          wrk2        <none>           <none>
# gpu-operator    gpu-operator-7754458dc4-zwzkn                                     1/1     Running    0          2m46s   10.244.0.5      ctl         <none>           <none>
# gpu-operator    nvidia-container-toolkit-daemonset-t2pq2                          1/1     Running    0          2m3s    10.244.2.13     wrk2        <none>           <none>
# gpu-operator    nvidia-dcgm-exporter-rq6fz                                        0/1     Init:0/1   0          2m3s    <none>          wrk2        <none>           <none>
# gpu-operator    nvidia-device-plugin-daemonset-ccz9n                              0/1     Init:0/1   0          2m3s    <none>          wrk2        <none>           <none>
# gpu-operator    nvidia-driver-daemonset-7vw55                                     1/1     Running    0          2m20s   10.244.2.11     wrk2        <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-gc-756547548-z4bzd     1/1     Running    0          2m46s   10.244.1.3      wrk2        <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-master-6776b5d9hrp66   1/1     Running    0          2m46s   10.244.0.6      ctl         <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-4gc7f           1/1     Running    0          2m45s   10.244.2.10     wrk2        <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-8gt56           1/1     Running    0          2m46s   10.244.0.4      ctl         <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-ssqwm           1/1     Running    0          2m45s   10.244.1.4      wrk2        <none>           <none>
# gpu-operator    nvidia-operator-validator-g4bkj                                   0/1     Init:0/4   0          2m3s    <none>          wrk2        <none>           <none>
# kai-scheduler   admission-5dbf884b4d-mxsn5                                        1/1     Running    0          12m     10.244.2.6      wrk2        <none>           <none>
# kai-scheduler   binder-6c88878778-m9nfd                                           1/1     Running    0          12m     10.244.2.8      wrk2        <none>           <none>
# kai-scheduler   kai-operator-f69885989-9g4zs                                      1/1     Running    0          12m     10.244.2.4      wrk2        <none>           <none>
# kai-scheduler   kai-scheduler-default-bcd44487-25vjq                              1/1     Running    0          12m     10.244.1.2      wrk2        <none>           <none>
# kai-scheduler   pod-grouper-d55f59cdb-xhl5n                                       1/1     Running    0          12m     10.244.2.9      wrk2        <none>           <none>
# kai-scheduler   podgroup-controller-6f5d4958b-zc4pd                               1/1     Running    0          12m     10.244.2.7      wrk2        <none>           <none>
# kai-scheduler   queue-controller-6976b95b58-ct5hm                                 1/1     Running    0          12m     10.244.2.5      wrk2        <none>           <none>
# kube-flannel    kube-flannel-ds-dnb6r                                             1/1     Running    0          31m     11.11.1.1       ctl         <none>           <none>
# kube-flannel    kube-flannel-ds-w7cg2                                             1/1     Running    0          30m     11.11.1.3       wrk2        <none>           <none>
# kube-flannel    kube-flannel-ds-zj2cf                                             1/1     Running    0          30m     11.11.1.2       wrk1        <none>           <none>
# kube-system     coredns-7d764666f9-9rflh                                          1/1     Running    0          35m     10.244.0.3      ctl         <none>           <none>
# kube-system     coredns-7d764666f9-kz7k2                                          1/1     Running    0          35m     10.244.0.2      ctl         <none>           <none>
# kube-system     etcd-ctl                                                          1/1     Running    0          35m     11.11.1.1       ctl         <none>           <none>
# kube-system     kube-apiserver-ctl                                                1/1     Running    0          35m     11.11.1.1       ctl         <none>           <none>
# kube-system     kube-controller-manager-ctl                                       1/1     Running    0          35m     11.11.1.1       ctl         <none>           <none>
# kube-system     kube-proxy-7dzww                                                  1/1     Running    0          35m     11.11.1.1       ctl         <none>           <none>
# kube-system     kube-proxy-9mtk5                                                  1/1     Running    0          30m     11.11.1.3       wrk2        <none>           <none>
# kube-system     kube-proxy-c74nc                                                  1/1     Running    0          30m     11.11.1.2       wrk1        <none>           <none>
# kube-system     kube-scheduler-ctl                                                1/1     Running    0          35m     11.11.1.1       ctl         <none>           <none>

kubectl logs -n gpu-operator nvidia-dcgm-exporter-rq6fz
kubectl logs -n gpu-operator nvidia-device-plugin-daemonset-ccz9n
kubectl logs -n gpu-operator nvidia-driver-daemonset-7vw55

# Check expected worker nodes are recognized as GPU nodes
kubectl get nodes -o wide --show-labels | grep nvidia

kubectl describe node wrk2 | grep -A10 nvidia.com/gpu
```

Checks after 10 min:

```shell
kubectl exec -n gpu-operator nvidia-device-plugin-daemonset-ccz9n -- nvidia-smi
# Defaulted container "nvidia-device-plugin" out of: nvidia-device-plugin, toolkit-validation (init)
# Wed Dec 31 21:26:34 2025
# +-----------------------------------------------------------------------------------------+
# | NVIDIA-SMI 550.163.01             Driver Version: 550.163.01    CUDA Version: 13.0     |
# +-----------------------------------------+------------------------+----------------------+
# | GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
# | Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
# |                                         |                        |               MIG M. |
# |=========================================+========================+======================|
# |   0  NVIDIA GeForce RTX 3060        Off |   00000000:05:00.0 Off |                    0 |
# | N/A   27C    P0             23W /  250W |       0MiB /  12288MiB |      0%      Default |
# |                                         |                        |                  N/A |
# +-----------------------------------------+------------------------+----------------------+

# +-----------------------------------------------------------------------------------------+
# | Processes:                                                                              |
# |  GPU   GI   CI              PID   Type   Process name                        GPU Memory |
# |        ID   ID                                                               Usage      |
# |=========================================================================================|
# |  No running processes found                                                             |
# +-----------------------------------------------------------------------------------------+
```
