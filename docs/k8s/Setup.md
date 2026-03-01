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

POD_NETWORK_CIDR="10.244.0.0/16"  # K8s default
SERVICE_CIDR="10.96.0.0/12"       # K8s default

 # Use your own node IPs
NODE_IPS="###.###.###,..."
```

If the nodes are in a private network, set the proxy configuration on all nodes:

```shell
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

ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/08_helm_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/09_helm-repositories_plays.yaml
```

### Create Cluster - Kueue

Add `kueue` support for advanced job admission and placement logic integrating with the native scheduler:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/10_kueue_plays.yaml
```

### Create Cluster - KAI Scheduler

Add `KAI Scheduler` support for advanced job admission and placement logic, replacing the native scheduler for certain workloads:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/11_kai-scheduler_plays.yaml
```

### Create Cluster - Nvidia GPU Operator

Add `Nvidia GPU Operator` to discover and configure GPU nodes:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/12_nvidia-gpu-operator_plays.yaml \
  -e use_proxy=no

# OR if the nodes are in a private network, set the job proxy configuration
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/12_nvidia-gpu-operator_plays.yaml \
  -e use_proxy=yes \
  -e http_proxy=${HTTP_PROXY} \
  -e pod_network_cidr=${POD_NETWORK_CIDR} \
  -e service_cidr=${SERVICE_CIDR} \
  -e node_ips=${NODE_IPS}
```

## Check Cluster

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/01_ping-hosts_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/02_k8s_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/03_k8s-nodes_plays.yaml
```

Check access to the cluster:

```shell
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
