# Tasks

## Prerequisites

- Install the required tools
  - [Tools.md](Tools.md)
- Setup your environment
  - [Setup.md](Setup.md)
- Create nodes
  - [Nodes.md](Nodes.md)

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

# Optionally sync again after updating dependencies versions in pyproject.toml
uv sync
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

See [K8s Cluster](k8s/README.md).

## Management Services

See [Management Services](mgt/README.md).
