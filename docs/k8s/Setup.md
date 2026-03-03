# K8s Cluster - Setup

First create an ad hoc cluster where you can schedule CPU work.
Next add support to the cluster for scheduling work on Nvidia GPUs.
Next add support to the cluster for advanced work scheduling and management.
Finally run tests to validate the cluster is working e.g. run CPU and GPU jobs.

- Setup prerequisites
- Setup Container Runtime
- Setup K8s
- Initialized the Cluster
  - Nodes will be in status `NotReady` until a Container Network Interface (CNI) network add-on is installed
- Setup Container Network Interface (CNI) network add-on
  - Cluster DNS (CoreDNS) will not start up before a CNI network implementation is installed
- Join workers to cluster
- Setup Helm and Helm Repositories
- Setup
  - Kueue
    - Advanced job admission and placement logic, integrating with the native scheduler
  - Setup KAI Scheduler
    - Advanced job admission and placement logic, replacing the native scheduler for certain workloads
    - GPU sharing
  - Setup Nvidia GPU Operator
    - If you don't have a GPU this setup will still work but no nodes with GPUs will be discovered
    - Discover and configure GPU nodes
    - GPU sharing
    - GPU monitoring

Initialize the virtual environment:

```shell
# Note 'uv' automatically infers the virtual environment
uv sync

# Activate the virtual environment
# -> For this shell alias see the 'Prerequisites' instructions
venv
```

Decide on the IPs for the cluster:

```shell
# If the nodes are in a private network make the network's proxy available to the nodes and K8s
HTTP_PROXY="http://proxy.example.com:####"

  # K8s defaults
POD_NETWORK_CIDR="10.244.0.0/16"
SERVICE_CIDR="10.96.0.0/12"

 # Use your own node IPs
NODE_IPS="###.###.###,..."
```

Check you can access all nodes:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/01_ping-hosts_plays.yaml
```

If the nodes are in a private network, set the proxy configuration on all nodes:

```shell
echo
echo "HTTP_PROXY=${HTTP_PROXY}"
echo "POD_NETWORK_CIDR=${POD_NETWORK_CIDR}"
echo "SERVICE_CIDR=${SERVICE_CIDR}"
echo "NODE_IPS=${NODE_IPS}"
echo

ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/01_proxies_plays.yaml \
  -e http_proxy=${HTTP_PROXY} \
  -e pod_network_cidr=${POD_NETWORK_CIDR} \
  -e service_cidr=${SERVICE_CIDR} \
  -e node_ips=${NODE_IPS}
```

## Create Cluster

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/02_prerequisites_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/03_container-runtime_cri-o_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/04_k8s_plays.yaml

ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/05_initialize-k8s_plays.yaml \
  -e pod_network_cidr=${POD_NETWORK_CIDR} \
  -e service_cidr=${SERVICE_CIDR}

ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/06_cni_flannel_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/07_join-workers-to-k8s_plays.yaml
```

Add `Helm` to the cluster:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/08_helm_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/09_helm-repositories_plays.yaml
```

Fetch the cluster's `.kube/config`:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/10_fetch-kube-config_plays.yaml
```

### Create Cluster - Check Resource Usage

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/04_k8s_nodes_resource-usage_plays.yaml
```

### Create Cluster - Kueue

Add `kueue` support for advanced job admission and placement logic integrating with the native scheduler:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/11_kueue_plays.yaml
```

### Create Cluster - KAI Scheduler

Add `KAI Scheduler` support for advanced job admission and placement logic, replacing the native scheduler for certain workloads:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/12_kai-scheduler_plays.yaml
```

### Create Cluster - Nvidia GPU Operator

Add `Nvidia GPU Operator` to discover and configure GPU nodes.

If the nodes are in a private network, set the job proxy configuration:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/13_nvidia-gpu-operator_plays.yaml \
  -e use_proxy=yes \
  -e http_proxy=${HTTP_PROXY} \
  -e pod_network_cidr=${POD_NETWORK_CIDR} \
  -e service_cidr=${SERVICE_CIDR} \
  -e node_ips=${NODE_IPS}
```

OR if the nodes are NOT in a private network:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/13_nvidia-gpu-operator_plays.yaml \
  -e use_proxy=no
```

## Check Cluster

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/01_ping-hosts_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/02_k8s_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/03_k8s-nodes_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/04_k8s_nodes_resource-usage_plays.yaml
```

Check access to the cluster:

```shell
cat ~/.kube/config | grep server:
#    server: https://10.10.228.153:6443

curl -k https://<K8s Cluster Controller IP>:6443/version
```

## Test Cluster

If the nodes are in a private network, set the job proxy configuration on all nodes:

```shell
# Note environment variables are populated on the Controller but used by all nodes
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/01_proxies_plays.yaml \
  -e http_proxy=${HTTP_PROXY} \
  -e pod_network_cidr=${POD_NETWORK_CIDR} \
  -e service_cidr=${SERVICE_CIDR} \
  -e node_ips=${NODE_IPS}
```

Create test data:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/02_kueue_test-data_plays.yaml \
  -e resource_flavor_name="default-flavor" \
  -e cluster_queue_name="research-grp" \
  -e cluster_queue_cpu_quota=9 \
  -e cluster_queue_memory_quota="36Gi" \
  -e cluster_queue_pods_quota=5 \
  -e cluster_queue_gpu_quota=2 \
  -e local_queue_name="training"

ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/03_kai-scheduler_test-data_plays.yaml \
  -e parent_queue_name="research-grp" \
  -e parent_queue_gpu_quota=-1 \
  -e parent_queue_gpu_limit=-1 \
  -e parent_queue_cpu_quota=-1 \
  -e parent_queue_cpu_limit=-1 \
  -e parent_queue_memory_quota=-1 \
  -e parent_queue_memory_limit=-1 \
  -e child_queue_name="training" \
  -e child_queue_gpu_quota=-1 \
  -e child_queue_gpu_limit=-1 \
  -e child_queue_cpu_quota=-1 \
  -e child_queue_cpu_limit=-1 \
  -e child_queue_memory_quota=-1 \
  -e child_queue_memory_limit=-1
```

### Test Cluster - Simple Jobs

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/simple/shell/01_create_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/simple/shell/02_display_plays.yaml

ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/simple/cpu/01_create_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/simple/cpu/02_display_plays.yaml

ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/simple/gpu/01_create_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/simple/gpu/02_display_plays.yaml
```

### Test Cluster - Kueue Jobs

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/kueue/gpu/01_create_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/kueue/gpu/02_display_plays.yaml
```

### Test Cluster - KAI Scheduler Jobs

Run `KAI Scheduler` jobs:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/kai-scheduler/gpu/01_create_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/jobs/kai-scheduler/gpu/02_display_plays.yaml
```

## Monitor Cluster

<details close>
<summary>Ambiguous container names</summary>

<ul>
  <li>Kueue</li>
  <ul>
    <li>manager</li>
  </ul>
  <li>KAI Scheduler</li>
  <ul>
    <li>admission</li>
    <li>binder</li>
    <li>operator</li>
    <li>pod-grouper</li>
    <li>queue-controller</li>
  </ul>
  <li>Nvidia GPU Operator</li>
  <ul>
    <li>gc</li>
    <li>master</li>
    <li>worker</li>
  </ul>
</ul>

<br>
<code>
sudo crictl ps -a -o json | jq '.containers[] | {id: .id, pod_name: .labels["io.kubernetes.pod.name"], namespace: .labels["io.kubernetes.pod.namespace"]}'
</code>

</details>

### Controller

`sudo crictl stats`:

```text
CONTAINER           NAME                      CPU %               MEM                 DISK                INODES              SWAP
0d29690fa3109       kube-proxy                0.00                17.63MB             174B                27                  0B
235df81326d43       coredns                   0.19                16.92MB             12B                 14                  0B
29343861404f9       kube-scheduler            1.13                34.88MB             12B                 10                  0B
5cefd6858e487       coredns                   0.22                17.15MB             12B                 14                  0B
65429109ac27e       kube-controller-manager   1.54                74.11MB             12B                 23                  0B
837256d3487a6       etcd                      3.70                75.99MB             12B                 14                  0B
87f3418681bf5       kube-apiserver            5.79                580.5MB             12B                 17                  0B
97bbf06d73a3e       master                    3.09                20.87MB             12B                 16                  0B
afea53e8d48c4       kube-flannel              1.55                18.08MB             162B                19                  0B
f001e1167ed14       gpu-operator              0.23                30.34MB             12B                 15                  0B
fb07350dbf5c6       worker                    0.00                20.34MB             12B                 25                  0B
```

### Worker - Compute

`sudo crictl stats`:

```text
CONTAINER           NAME                    CPU %               MEM                 DISK                INODES              SWAP
2e89c0c7d3376       kube-proxy              0.00                20.55MB             174B                27                  0B
5259a0a7edcf4       kube-flannel            1.50                19.76MB             162B                19                  0B
aeced8effb486       kai-scheduler-default   0.19                60.48MB             12B                 12                  0B
bef91cd70da1e       manager                 0.26                49.86MB             12B                 18                  0B
e1ca08d7b06ba       worker                  0.00                21.96MB             12B                 25                  0B
```

### Worker - GPU

`sudo crictl stats`:

```text
CONTAINER           NAME                           CPU %               MEM                 DISK                INODES              SWAP
082ca0cf78e4c       gpu-feature-discovery          0.00                41.3MB              10.01kB             125                 0B
09560abb5a92c       queue-controller               0.04                12.45MB             12B                 13                  0B
183335c6a6471       binder                         0.06                20.59MB             12B                 10                  0B
28185f61f2e17       nvidia-container-toolkit-ctr   0.00                22.32MB             12B                 31                  0B
286240a20e551       nvidia-dcgm-exporter           0.77                402.8MB             164.5kB             127                 0B
52a6afd165f55       nvidia-device-plugin           0.00                25.2MB              10.01kB             131                 0B
63e9a48a4dc5c       worker                         0.00                22.19MB             12B                 25                  0B
82138fb7f6730       nvidia-operator-validator      0.00                2.363MB             12B                 15                  0B
9b07ad5b52058       admission                      0.75                16.8MB              12B                 13                  0B
b07055c851285       operator                       0.12                33.63MB             12B                 10                  0B
b480f565a5dfa       gc                             0.00                12.07MB             12B                 16                  0B
b5a4bdf97174c       podgroup-controller            0.04                12.29MB             12B                 13                  0B
b7160b10363f0       kube-proxy                     0.00                19.13MB             174B                27                  0B
de417c1dd26d7       nvidia-driver-ctr              0.00                1.493GB             1.477GB             33449               0B
e7ee5f9c760f8       pod-grouper                    0.07                12.08MB             12B                 10                  0B
f219babeeba5f       kube-flannel                   2.52                19.86MB             162B                19                  0B
```
