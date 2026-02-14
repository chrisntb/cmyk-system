# CMYK System

> Felix Felicis - VS Code workspace [cmyk-system.code-workspace](cmyk-system.code-workspace)

> [CMYK](https://github.com/chrisntb/cmyk) - Engineering and architecture documentation

Infrastructure configuration management using Ansible playbooks.

A [tutorial](docs/k8s/cluster/README.md) explaining how to manually create a K8s cluster which supports queues and scheduling work on compute and GPU nodes. A [tutorial](docs/slurm/cluster/README.md) explaining how to manually create a Slurm cluster which supports queues and scheduling work on compute and GPU nodes.

## Documentation

See [docs/README.md](docs/README.md).

## Prerequisites

See [docs/Tools.md](docs/Tools.md) for the required tools.

To setup your Ansible environment, see [docs/Setup.md](docs/Setup.md).
After setup, create nodes for the clusters, see [docs/Nodes.md](docs/Nodes.md).

## Playbooks

Before running tasks, make sure you have completed the `Prerequisites` section above.

Before running a `playbook` make sure you have activated the virtual environment `venv` and installed the dependencies `uv sync`.

### Checks

Initialize the virtual environment:

```shell
# Note 'uv' automatically infers the virtual environment
uv sync

venv
```

Check for outdated dependencies:

```shell
uv tree --outdated --depth=1
```

Check tools:

```shell
ansible --version
ansible-playbook --version
```

Try some ad hoc commands:

```shell
ansible -i inventory/dev/hosts.yaml cluster -m command -a 'cat /etc/hosts'
ansible -i inventory/dev/hosts.yaml cluster -m command -a 'ip -br a'
```

Run the node health checks:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/check_ping-hosts_plays.yaml
```

### Setup K8s Cluster

- Setup prerequisites
- Setup Container Runtime
- Setup K8s
- Initialized the Cluster
  - Nodes will be in status `NotReady` until a Container Network Interface (CNI) network add-on is installed
- Setup Container Network Interface (CNI) network add-on
  - Cluster DNS (CoreDNS) will not start up before a CNI network implementation is installed
- Join workers to cluster
- Setup Helm
- Setup Helm Repositories
- Setup Kueue
  - Advanced job admission and placement logic, integrating with the native scheduler
- Setup KAI Scheduler
  - Advanced job admission and placement logic, replacing the native scheduler for certain workloads
  - GPU sharing
- Setup Nvidia GPU Operator
  - If you don't have a GPU this setup will still work but no nodes with GPUs will be discovered
  - Discover and configure GPU nodes
  - GPU sharing
  - GPU monitoring
- TODO
  - Setup CloudNativePG
  - Setup Gateway API

Initialize the virtual environment:

```shell
# Note 'uv' automatically infers the virtual environment
uv sync

venv
```

Create the cluster:

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-01_prerequisites_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-02_container-runtime_cri-o_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-03_k8s_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-04_initialize-k8s_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-05_cni_flannel_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-06_join-workers-to-k8s_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-07_helm_plays.yaml
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-08_helm-repositories_plays.yaml

# If you want to use Kueue
# -> Advanced job admission and placement logic, integrating with the native scheduler
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-09_kueue_plays.yaml

# If you want to use the KAI scheduler
# -> Advanced job admission and placement logic, replacing the native scheduler for certain workloads
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-10_kai-scheduler_plays.yaml

# If you have GPU workers
# Discover and configure GPU nodes
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-11_nvidia-gpu-operator_plays.yaml

# Check things look OK
ansible-playbook -i inventory/dev/hosts.yaml playbooks/check_k8s_plays.yaml
```

Check access to the cluster:

```shell
curl -k https://<K8s Cluster Controller IP>:6443/version
```

### Setup Slurm Cluster

TODO

## TODO

### Postgres Operator - CloudNativePG

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup-11_cloud-native-pg_plays.yaml
```

### Gateway API

```shell
# See https://github.com/kubernetes-sigs/gateway-api/releases
kubectl apply --server-side -f https://github.com/kubernetes-sigs/gateway-api/releases/download/v1.4.1/experimental-install.yaml
# "Warning: Regarding the Experimental CRDs - please note that the experimental CRDs for this release are too large for a standard kubectl apply. You may receive an error like metadata.annotations: Too long: may not be more than 262144 bytes"

helm repo add haproxy-ingress https://haproxytech.github.io/helm-charts
helm repo update
helm install haproxy-ingress haproxy-ingress/haproxy-ingress \
  --namespace gateway \
  --create-namespace \
  --set gateways.enabled=true \
  --set controller.service.type=LoadBalancer

# This deploys the controller with GatewayClass haproxy ready, provisions a LoadBalancer Service, and enables Gateway API parsing
kubectl get pods,svc -n gateway
kubectl get gatewayclass haproxy
```
