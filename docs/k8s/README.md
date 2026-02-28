# K8s Cluster

- [Architecture](Architecture.md)
- [Setup](Setup.md)

This is an standalone cluster with all required supporting services in order that there are no dependencies/blockers.
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

There is also an example of creating the cluster manually, which makes the `moving parts` clearer, see [K8s Cluster - Manual Example](manual-example/README.md).
