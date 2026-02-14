# K8s Cluster Example - Overview

This is an ad hoc standalone cluster with all required supporting services in order that there are no dependencies/blockers.
The machines should be wiped when you are finished with the cluster.

The cluster demonstrates:

- Using Kueue
  - Advanced job admission and placement logic, integrating with the native scheduler
- Using KAI Scheduler
  - Advanced job admission and placement logic, replacing the native scheduler for certain workloads
  - GPU sharing
- Using Nvidia GPU Operator
  - Discover and configure GPU nodes
  - GPU sharing
  - GPU monitoring
- Monitoring using Prometheus and Grafana
  - Nvidia DCGM used for GPU metrics
- Tests you can run yourself :)

## Nodes

Ubuntu 24.04 installed on nodes:

- Controller `ctl`
  - `11.11.1.1`
- Compute Worker `wrk1`
  - `11.11.1.2`
- GPU Worker `wrk2`
  - `11.11.1.3`

Convenient `.ssh/config`:

```text
Host *
    ServerAliveInterval 30
    ServerAliveCountMax 2

Host j
    HostName 11.11.1.0
    User <Username>
    IdentityFile ~/.ssh/<Private Key>
    IdentitiesOnly yes


Host ctl
    HostName 11.11.1.1
    User ubuntu
    ProxyJump j
    IdentityFile ~/.ssh/<Private Key>
    IdentitiesOnly yes

Host wrk1
    HostName 11.11.1.2
    User ubuntu
    ProxyJump j
    IdentityFile ~/.ssh/<Private Key>
    IdentitiesOnly yes

Host wrk2
    HostName 11.11.1.3
    User ubuntu
    ProxyJump j
    IdentityFile ~/.ssh/<Private Key>
    IdentitiesOnly yes
```

### Resources

Make sure you are using nodes that have large enough disks.
After installing K8s the node disk usage `sudo du -h -d 1 /` was as follows:

- Controller `15G`
- Worker - Compute `20G`
- Worker - GPU `36G`

Make sure you are using nodes that have enough cores and memory.
We received these errors when initially testing with very small virtual machines:

```shell
[ERROR NumCPU]: the number of available CPUs 1 is less than the required 2
[ERROR Mem]: the system RAM (955 MB) is less than the minimum 1700 MB
```

#### Cluster Activity

Nodes have a number of pods running when using the KAI Scheduler and Nvidia GPU Operator.
The resources consumed by these pods will need to be throughly tested because they will impact the resources available for jobs.
Initial testing indicates we can expect to lose a small fraction of a core and ~0.5GB of memory.

Controller node: `kubectl get pods --all-namespaces -o wide | grep ctl`:

```shell
gpu-operator    gpu-operator-7754458dc4-r84nt                                     1/1     Running     0          14m   10.244.0.4      ctl   <none>           <none>
gpu-operator    nvidia-gpu-operator-node-feature-discovery-master-6776b5d9fx4mj   1/1     Running     0          14m   10.244.0.6      ctl   <none>           <none>
gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-5pnsn           1/1     Running     0          14m   10.244.0.5      ctl   <none>           <none>
kube-flannel    kube-flannel-ds-45rdn                                             1/1     Running     0          18m   11.11.1.1       ctl   <none>           <none>
kube-system     coredns-7d764666f9-g7cdj                                          1/1     Running     0          32m   10.244.0.2      ctl   <none>           <none>
kube-system     coredns-7d764666f9-vgnrw                                          1/1     Running     0          32m   10.244.0.3      ctl   <none>           <none>
kube-system     etcd-ctl                                                          1/1     Running     0          33m   11.11.1.1       ctl   <none>           <none>
kube-system     kube-apiserver-ctl                                                1/1     Running     0          33m   11.11.1.1       ctl   <none>           <none>
kube-system     kube-controller-manager-ctl                                       1/1     Running     0          33m   11.11.1.1       ctl   <none>           <none>
kube-system     kube-proxy-gg97m                                                  1/1     Running     0          32m   11.11.1.1       ctl   <none>           <none>
kube-system     kube-scheduler-ctl                                                1/1     Running     0          33m   11.11.1.1       ctl   <none>           <none>
```

GPU worker node: `kubectl get pods --all-namespaces -o wide | grep wrk2`:

```shell
gpu-operator    gpu-feature-discovery-bjlq7                                       1/1     Running     0          2m53s   10.244.1.16     wrk2   <none>           <none>
gpu-operator    nvidia-container-toolkit-daemonset-sxbbp                          1/1     Running     0          2m53s   10.244.1.15     wrk2   <none>           <none>
gpu-operator    nvidia-cuda-validator-hwxr4                                       0/1     Completed   0          52s     10.244.1.18     wrk2   <none>           <none>
gpu-operator    nvidia-dcgm-exporter-k26ww                                        1/1     Running     0          2m53s   10.244.1.19     wrk2   <none>           <none>
gpu-operator    nvidia-device-plugin-daemonset-qknbn                              1/1     Running     0          2m53s   10.244.1.20     wrk2   <none>           <none>
gpu-operator    nvidia-driver-daemonset-7csdl                                     1/1     Running     0          3m15s   10.244.1.13     wrk2   <none>           <none>
gpu-operator    nvidia-gpu-operator-node-feature-discovery-gc-756547548-8sr2w     1/1     Running     0          3m56s   10.244.1.11     wrk2   <none>           <none>
gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-plmrg           1/1     Running     0          3m55s   10.244.1.12     wrk2   <none>           <none>
gpu-operator    nvidia-operator-validator-2xw8w                                   1/1     Running     0          2m53s   10.244.1.17     wrk2   <none>           <none>
kai-scheduler   admission-597666bdd7-gwvqw                                        1/1     Running     0          4m57s   10.244.1.8      wrk2   <none>           <none>
kai-scheduler   binder-6c854cdfc6-hqb9c                                           1/1     Running     0          4m57s   10.244.1.5      wrk2   <none>           <none>
kai-scheduler   kai-operator-586d5f8f47-knl2r                                     1/1     Running     0          5m5s    10.244.1.4      wrk2   <none>           <none>
kai-scheduler   kai-scheduler-default-68fd8f7d49-xpzjr                            1/1     Running     0          4m56s   10.244.1.10     wrk2   <none>           <none>
kai-scheduler   pod-grouper-85b8d77-gqlqt                                         1/1     Running     0          4m57s   10.244.1.7      wrk2   <none>           <none>
kai-scheduler   podgroup-controller-7cbbd9d567-hvzcc                              1/1     Running     0          4m57s   10.244.1.6      wrk2   <none>           <none>
kai-scheduler   queue-controller-7877bfc9c9-r67k5                                 1/1     Running     0          4m57s   10.244.1.9      wrk2   <none>           <none>
kube-flannel    kube-flannel-ds-n6g8v                                             1/1     Running     0          7m25s   11.11.1.3       wrk2   <none>           <none>
kube-system     kube-proxy-29nkz                                                  1/1     Running     0          7m25s   11.11.1.3       wrk2   <none>           <none>
```

## Setup

First create an ad hoc cluster where you can schedule CPU work.
Next add support to the cluster for scheduling work on Nvidia GPUs.
Next add support to the cluster for advanced work scheduling and management.
Finally run tests to validate the cluster is working e.g. run CPU and GPU jobs.

Create an ad hoc cluster:

- Create an ad hoc cluster where you can schedule CPU work
  - [Setup](02_Setup.md)
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
- Multi-tenant CPU/GPU HPC service: "...science no longer necessarily begins with a microscope, or a chalkboard. It begins with a log-in screen. Maybe an API key. A dashboard with the number of available credits, how long your job queue is, perhaps a warning about GPU demand. Welcome to science in 2025." [https://www.hpcwire.com/2025/12/18/how-data-is-changing-science-part-3-the-infrastructure-layer/]
  - Buy e.g. [Run.AI](https://www.nvidia.com/en-us/software/run-ai/)
  - Build using the K8s API
  - Hijack e.g. [SkyPilot](https://github.com/skypilot-org)

### Proxy

With the cluster setup, a proxy can be used for convenience.

```shell
export JUMP_IP="11.11.1.0"
export JUMP_KEY="~/.ssh/<Private Key>"
export JUMP_USER_NAME="<Your Username>"

export K8S_CONTROLLER_IP="11.11.1.1"

ssh -i ${JUMP_KEY} -f -C -q -N -L 16443:${K8S_CONTROLLER_IP}:6443 ${JUMP_USER_NAME}@${JUMP_IP}
curl -k https://127.0.0.1:16443/version
```

### Lens

With the cluster setup, Lens can be used for cluster visibility.

First configure an SSH proxy, see previous section.

Next add a Kubeconfig to Lens: copy one from the controller `~/.kube/config` and modify it so it works with the SSH proxy.

Controller `~/.kube/config`:

```text
apiVersion: v1
clusters:
- cluster:
    certificate-authority-data: LS...0K
    server: https://11.11.11.1:6443
  name: kubernetes
contexts:
- context:
    cluster: kubernetes
    user: kubernetes-admin
  name: kubernetes-admin@kubernetes
current-context: kubernetes-admin@kubernetes
kind: Config
users:
- name: kubernetes-admin
  user:
    client-certificate-data: LS...0K
    client-key-data: LS...Cg==
```

Modified `~/.kube/config` allowing Lens access to the cluster via an SSH proxy:

```text
apiVersion: v1
clusters:
- cluster:
    insecure-skip-tls-verify: true
    server: https://127.0.0.1:16443
  name: kubernetes
contexts:
- context:
    cluster: kubernetes
    user: kubernetes-admin
  name: kubernetes-admin@kubernetes
current-context: kubernetes-admin@kubernetes
kind: Config
users:
- name: kubernetes-admin
  user:
    client-certificate-data: LS...0K
    client-key-data: LS...Cg==
```

### SkyPilot

With the cluster setup, SkyPilot (`https://github.com/skypilot-org`) can be used to offer the cluster as a service.
It includes support for Kueue i.e. advanced job admission and placement logic.
SkyPilot is a Ray based abstraction over various types of infrastructure services, including K8s.
If the intention is to use K8s, it is better to just use K8s: SkyPilot brings a lot of baggage because it is supports many different targets and will introduce friction because it facades the K8s API.
