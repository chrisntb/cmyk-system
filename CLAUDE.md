# Context

- Nodes running Ubuntu 24.04
- Configuration management using Ansible Playbooks
- Playbooks setup nodes so Kubernetes can be installed
- Playbooks install Kubernetes to form a cluster from the nodes
- Playbooks install various features into the Kubernetes cluster to implement a multi-tenant HPC cluster where CPU and GPU work can be submitted by users, run fairly and the results distributed conveniently
  - See [Kueue](https://kueue.sigs.k8s.io/), Kubernetes-native Job Queueing
