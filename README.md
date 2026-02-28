# CMYK System

> Felix Felicis - VS Code workspace [cmyk-system.code-workspace](cmyk-system.code-workspace)

> [CMYK](https://github.com/chrisntb/cmyk) - Engineering and architecture documentation

Infrastructure configuration management using Ansible playbooks.

A [tutorial](docs/k8s/cluster/README.md) explaining how to manually create a K8s cluster which supports queues and scheduling work on compute and GPU nodes.

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

# Activate the virtual environment
# -> For this shell alias see the 'Prerequisites' instructions
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
ansible-playbook -i inventory/dev/hosts.yaml playbooks/checks/01_ping-hosts_plays.yaml
```

## K8s Cluster

See [K8s Cluster](./docs/k8s/README.md).

## Management Services

See [Management Services](./docs/mgt/README.md).
