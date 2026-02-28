# K8s Cluster - Manual Example - Setup Monitoring

After you have setup the cluster, see [Setup](02_Setup.md), setup monioring.

These instruction were adapted from the official installation instructions:

- `https://github.com/prometheus-community/helm-charts`

In short monitoring is setup using the standard Helm chart.

## On Controller Node

Create setup script: setup-monitoring.sh

```shell
cat <<'EOF' > setup-monitoring.sh
#!/bin/bash
set -e
# ********
#
# USAGE
# setup-monitoring.sh
#
# ********

# See https://github.com/prometheus-community/helm-charts/releases
export KUBE_PROMETHEUS_STACK_VERSION="80.8.0"

echo
echo "-------- SETTING UP --------"
echo "KUBE_PROMETHEUS_STACK_VERSION=${KUBE_PROMETHEUS_STACK_VERSION}"
echo "-"
echo

echo
echo "-------- SETTING UP - Helm Chart --------"
helm repo add prometheus-community https://prometheus-community.github.io/helm-charts \
  && helm repo update
echo "-"
echo

echo
echo "-------- SETTING UP - Install --------"
helm install kube-prometheus-stack prometheus-community/kube-prometheus-stack --namespace monitoring --create-namespace --version ${KUBE_PROMETHEUS_STACK_VERSION}
echo "-"
echo

echo
echo "-------- COMPLETE --------"
echo

EOF
```

Run setup script:

```shell
chmod 700 setup-monitoring.sh

# Note running as the 'ubuntu' user
./setup-monitoring.sh 2>&1 | tee -a setup-monitoring.sh.log
```

Checks:

```shell
helm repo list
# NAME                  URL
# nvidia                https://helm.ngc.nvidia.com/nvidia
# prometheus-community  https://prometheus-community.github.io/helm-charts

helm list -A
# NAME                  NAMESPACE     REVISION UPDATED                                  STATUS    CHART                         APP VERSION
# kai-scheduler         kai-scheduler 1        2025-12-31 21:09:09.906667711 +0000 UTC  deployed  kai-scheduler-v0.12.1         v0.12.1
# kube-prometheus-stack monitoring    1        2025-12-31 22:12:28.176820462 +0000 UTC  deployed  kube-prometheus-stack-80.8.0  v0.87.1
# nvidia-gpu-operator   gpu-operator  1        2025-12-31 21:19:19.875126086 +0000 UTC  deployed  gpu-operator-v25.10.1         v25.10.1

helm get values kube-prometheus-stack -n monitoring -o yaml
# null

kubectl get pods --all-namespaces -o wide
# NAMESPACE       NAME                                                              READY   STATUS      RESTARTS   AGE     IP              NODE   NOMINATED NODE   READINESS GATES
# default         test-container-environment                                        0/1     Completed   0          47m     10.244.1.5      wrk1   <none>           <none>
# default         test-cpu-python                                                   0/1     Completed   0          45m     10.244.1.6      wrk1   <none>           <none>
# default         test-gpu-python                                                   0/1     Completed   0          42m     10.244.2.19     wrk2   <none>           <none>
# default         test-queue-gpu                                                    0/1     Completed   0          35m     10.244.2.20     wrk2   <none>           <none>
# gpu-operator    gpu-feature-discovery-c64lx                                       1/1     Running     0          58m     10.244.2.16     wrk2   <none>           <none>
# gpu-operator    gpu-operator-7754458dc4-zwzkn                                     1/1     Running     0          59m     10.244.0.5      ctl    <none>           <none>
# gpu-operator    nvidia-container-toolkit-daemonset-t2pq2                          1/1     Running     0          58m     10.244.2.13     wrk2   <none>           <none>
# gpu-operator    nvidia-cuda-validator-vzvxh                                       0/1     Completed   0          56m     10.244.2.18     wrk2   <none>           <none>
# gpu-operator    nvidia-dcgm-exporter-rq6fz                                        1/1     Running     0          58m     10.244.2.14     wrk2   <none>           <none>
# gpu-operator    nvidia-device-plugin-daemonset-ccz9n                              1/1     Running     0          58m     10.244.2.15     wrk2   <none>           <none>
# gpu-operator    nvidia-driver-daemonset-7vw55                                     1/1     Running     0          59m     10.244.2.11     wrk2   <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-gc-756547548-z4bzd     1/1     Running     0          59m     10.244.1.3      wrk1   <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-master-6776b5d9hrp66   1/1     Running     0          59m     10.244.0.6      ctl    <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-4gc7f           1/1     Running     0          59m     10.244.2.10     wrk2   <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-8gt56           1/1     Running     0          59m     10.244.0.4      ctl    <none>           <none>
# gpu-operator    nvidia-gpu-operator-node-feature-discovery-worker-ssqwm           1/1     Running     0          59m     10.244.1.4      wrk1   <none>           <none>
# gpu-operator    nvidia-operator-validator-g4bkj                                   1/1     Running     0          58m     10.244.2.17     wrk2   <none>           <none>
# kai-scheduler   admission-5dbf884b4d-mxsn5                                        1/1     Running     0          69m     10.244.2.6      wrk2   <none>           <none>
# kai-scheduler   binder-6c88878778-m9nfd                                           1/1     Running     0          69m     10.244.2.8      wrk2   <none>           <none>
# kai-scheduler   kai-operator-f69885989-9g4zs                                      1/1     Running     0          69m     10.244.2.4      wrk2   <none>           <none>
# kai-scheduler   kai-scheduler-default-bcd44487-25vjq                              1/1     Running     0          69m     10.244.1.2      wrk1   <none>           <none>
# kai-scheduler   pod-grouper-d55f59cdb-xhl5n                                       1/1     Running     0          69m     10.244.2.9      wrk2   <none>           <none>
# kai-scheduler   podgroup-controller-6f5d4958b-zc4pd                               1/1     Running     0          69m     10.244.2.7      wrk2   <none>           <none>
# kai-scheduler   queue-controller-6976b95b58-ct5hm                                 1/1     Running     0          69m     10.244.2.5      wrk2   <none>           <none>
# kube-flannel    kube-flannel-ds-dnb6r                                             1/1     Running     0          88m     11.11.1.1       ctl    <none>           <none>
# kube-flannel    kube-flannel-ds-w7cg2                                             1/1     Running     0          87m     11.11.1.3       wrk2   <none>           <none>
# kube-flannel    kube-flannel-ds-zj2cf                                             1/1     Running     0          87m     11.11.1.2       wrk1   <none>           <none>
# kube-system     coredns-7d764666f9-9rflh                                          1/1     Running     0          92m     10.244.0.3      ctl    <none>           <none>
# kube-system     coredns-7d764666f9-kz7k2                                          1/1     Running     0          92m     10.244.0.2      ctl    <none>           <none>
# kube-system     etcd-ctl                                                          1/1     Running     0          92m     11.11.1.1       ctl    <none>           <none>
# kube-system     kube-apiserver-ctl                                                1/1     Running     0          92m     11.11.1.1       ctl    <none>           <none>
# kube-system     kube-controller-manager-ctl                                       1/1     Running     0          92m     11.11.1.1       ctl    <none>           <none>
# kube-system     kube-proxy-7dzww                                                  1/1     Running     0          92m     11.11.1.1       ctl    <none>           <none>
# kube-system     kube-proxy-9mtk5                                                  1/1     Running     0          87m     11.11.1.3       wrk2   <none>           <none>
# kube-system     kube-proxy-c74nc                                                  1/1     Running     0          87m     11.11.1.2       wrk1   <none>           <none>
# kube-system     kube-scheduler-ctl                                                1/1     Running     0          92m     11.11.1.1       ctl    <none>           <none>
# monitoring      alertmanager-kube-prometheus-stack-alertmanager-0                 2/2     Running     0          5m44s   10.244.1.12     wrk1   <none>           <none>
# monitoring      kube-prometheus-stack-grafana-76466dc4bc-z5vbl                    3/3     Running     0          5m56s   10.244.1.9      wrk1   <none>           <none>
# monitoring      kube-prometheus-stack-kube-state-metrics-59b9d4c6b-v56w5          1/1     Running     0          5m56s   10.244.1.10     wrk1   <none>           <none>
# monitoring      kube-prometheus-stack-operator-7cc4b8bd78-cl792                   1/1     Running     0          5m56s   10.244.1.8      wrk1   <none>           <none>
# monitoring      kube-prometheus-stack-prometheus-node-exporter-bq2md              1/1     Running     0          5m56s   11.11.1.3       wrk2   <none>           <none>
# monitoring      kube-prometheus-stack-prometheus-node-exporter-h4wsh              1/1     Running     0          5m56s   11.11.1.1       ctl    <none>           <none>
# monitoring      kube-prometheus-stack-prometheus-node-exporter-w7wkr              1/1     Running     0          5m56s   11.11.1.2       wrk1   <none>           <none>
# monitoring      prometheus-kube-prometheus-stack-prometheus-0                     2/2     Running     0          5m44s   10.244.1.13     wrk1   <none>           <none>

kubectl --namespace monitoring get pods -l "release=kube-prometheus-stack"
# NAME                                                       READY   STATUS    RESTARTS   AGE
# kube-prometheus-stack-kube-state-metrics-59b9d4c6b-v56w5   1/1     Running   0          6m28s
# kube-prometheus-stack-operator-7cc4b8bd78-cl792            1/1     Running   0          6m28s
# kube-prometheus-stack-prometheus-node-exporter-bq2md       1/1     Running   0          6m28s
# kube-prometheus-stack-prometheus-node-exporter-h4wsh       1/1     Running   0          6m28s
# kube-prometheus-stack-prometheus-node-exporter-w7wkr       1/1     Running   0          6m28s
```

Output:

```text
"prometheus-community" has been added to your repositories
Hang tight while we grab the latest from your chart repositories...
...Successfully got an update from the "nvidia" chart repository
...Successfully got an update from the "prometheus-community" chart repository
Update Complete. ⎈Happy Helming!⎈
...
NAME: kube-prometheus-stack
LAST DEPLOYED: Wed Dec 31 22:12:28 2025
NAMESPACE: monitoring
STATUS: deployed
REVISION: 1
DESCRIPTION: Install complete
NOTES:
kube-prometheus-stack has been installed. Check its status by running:
  kubectl --namespace monitoring get pods -l "release=kube-prometheus-stack"

Get Grafana 'admin' user password by running:

  kubectl --namespace monitoring get secrets kube-prometheus-stack-grafana -o jsonpath="{.data.admin-password}" | base64 -d ; echo

Access Grafana local instance:

  export POD_NAME=$(kubectl --namespace monitoring get pod -l "app.kubernetes.io/name=grafana,app.kubernetes.io/instance=kube-prometheus-stack" -oname)
  kubectl --namespace monitoring port-forward $POD_NAME 3000

Get your grafana admin user password by running:

  kubectl get secret --namespace monitoring -l app.kubernetes.io/component=admin-secret -o jsonpath="{.items[0].data.admin-password}" | base64 --decode ; echo


Visit https://github.com/prometheus-operator/kube-prometheus for instructions on how to create & configure Alertmanager and Prometheus instances using the Operator.
```

Expose the Prometheus `service` so Grafana can access it. Note if you need to debug, you can now temporarily access this `service` using an SSH proxy:

```shell
kubectl port-forward -n monitoring svc/kube-prometheus-stack-prometheus 9090:9090 --address 0.0.0.0
# curl http://11.11.1.1:9090/
```

As mentioned in the setup log, you can get the Grafana 'admin' user password by running:

```shell
kubectl --namespace monitoring get secrets kube-prometheus-stack-grafana -o jsonpath="{.data.admin-password}" | base64 -d ; echo
```

### Nvidia DCGM Exporter

For Prometheus to scrape Nvidia DCGM metrics, it must be made aware of DCGM. For Prometheus outside of K8s you do this my configuring the DCGM endpoint as a `target` in Prometheus’s configuration file `prometheus.yml`. In K8s you use a `ServiceMonitor`.

Create the `ServiceMonitor` specification for the nvidia-dcgm-exporter `service`:

Determine the labels you will need for the `ServiceMonitor`:

```shell
kubectl describe svc -n gpu-operator nvidia-dcgm-exporter
# ...
# Labels:                   app=nvidia-dcgm-exporter
# ...
# Port:                     gpu-metrics  9400/TCP
# ...

kubectl describe svc -n monitoring kube-prometheus-stack-prometheus
# ...
# Labels:                   release=kube-prometheus-stack
# ...
```

Create the `ServiceMonitor` specification:

```shell
cat <<'EOF' > servicemonitor-nvidia-dcgm-exporter.yaml
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: nvidia-dcgm-exporter
  namespace: gpu-operator
  labels:
    release: kube-prometheus-stack  # Match your 'kube-prometheus-stack-prometheus' service Label 'release'
spec:
  selector:
    matchLabels:
      app: nvidia-dcgm-exporter  # Match your 'nvidia-dcgm-exporter' service Label 'app'
  endpoints:
  - port: gpu-metrics
    interval: 15s
    path: /metrics  # Match your 'nvidia-dcgm-exporter' service 'Port'
EOF
```

Apply the `ServiceMonitor` specification for the nvidia-dcgm-exporter `service`:

```shell
kubectl apply -f servicemonitor-nvidia-dcgm-exporter.yaml
# kubectl delete servicemonitors -n gpu-operator nvidia-dcgm-exporter

kubectl get servicemonitor -n gpu-operator
# NAME                   AGE
# nvidia-dcgm-exporter   11s

kubectl get endpoints -n gpu-operator -l app=nvidia-dcgm-exporter
# Warning: v1 Endpoints is deprecated in v1.33+; use discovery.k8s.io/v1 EndpointSlice
# NAME                   ENDPOINTS          AGE
# nvidia-dcgm-exporter   10.244.2.14:9400   63m
```

Expose the nvidia-dcgm-exporter `service` so Prometheus can access it.
Note if you need to debug, you can now temporarily access this `service` using an SSH proxy:

```shell
kubectl port-forward -n gpu-operator svc/nvidia-dcgm-exporter 9400:9400 --address 0.0.0.0
# curl http://11.11.1.1:9400/metrics
```

#### Debug

Check network connectivity between the Prometheus `service` and the nvidia-dcgm-exporter `service`:

```shell
PROMETHEUS_POD=$(kubectl get pods -n monitoring -l app.kubernetes.io/name=prometheus -o name | head -1)
kubectl exec -n monitoring ${PROMETHEUS_POD} -- wget -qO- --timeout=5 nvidia-dcgm-exporter.gpu-operator.svc.cluster.local:9400/metrics | grep DCGM_FI_DEV_GPU_UTIL
# # HELP DCGM_FI_DEV_GPU_UTIL GPU utilization (in %).
# # TYPE DCGM_FI_DEV_GPU_UTIL gauge
# DCGM_FI_DEV_GPU_UTIL{gpu="0",UUID="GPU-b43d9221-8a75-4432-9cd6-64b97c0a4c90",pci_bus_id="00000000:05:00.0",device="nvidia0",modelName="NVIDIA GeForce RTX 3060",Hostname="wrk2",DCGM_FI_DRIVER_VERSION="550.163.01"} 0
```

If there is no network connectivity between the Prometheus `service` and the nvidia-dcgm-exporter `service`, you will need to add an egress `NetworkPolicy` for the `monitoring` `namespace` and an ingress for the `gpu-operator` `namespace`:

#### Optional Egress/Ingress NetworkPolicies

Create an egress `NetworkPolicy` specification for the Prometheus `service` to access the `gpu-operator` `namespace`:

```shell
cat <<'EOF' > network-policy-prometheus-egress-gpu-operator.yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: prometheus-egress-gpu-operator
  namespace: monitoring
spec:
  podSelector:
    matchLabels:
      app.kubernetes.io/name: prometheus
  policyTypes:
  - Egress
  egress:
  - to:
    - namespaceSelector:
        matchLabels:
          kubernetes.io/metadata.name: gpu-operator
    ports:
    - protocol: TCP
      port: 9400
EOF
```

Apply the `NetworkPolicy` specification for the Prometheus `service`:

```shell
kubectl apply -f network-policy-prometheus-egress-gpu-operator.yaml

kubectl get networkpolicy -n monitoring
# NAME                             POD-SELECTOR                        AGE
# prometheus-egress-gpu-operator   app.kubernetes.io/name=prometheus   5m19s

kubectl get pods -n monitoring -l app.kubernetes.io/name=prometheus --show-labels
# NAME                                            READY   STATUS    RESTARTS   AGE   LABELS
# prometheus-kube-prometheus-stack-prometheus-0   2/2     Running   0          16m   app.kubernetes.io/instance=kube-prometheus-stack-prometheus,app.kubernetes.io/managed-by=prometheus-operator,app.kubernetes.io/name=prometheus,app.kubernetes.io/version=3.8.1,apps.kubernetes.io/pod-index=0,controller-revision-hash=prometheus-kube-prometheus-stack-prometheus-6b5dfcbcdd,operator.prometheus.io/name=kube-prometheus-stack-prometheus,operator.prometheus.io/shard=0,prometheus=kube-prometheus-stack-prometheus,statefulset.kubernetes.io/pod-name=prometheus-kube-prometheus-stack-prometheus-0

kubectl describe networkpolicy -n monitoring prometheus-egress-gpu-operator
# Name:         prometheus-egress-gpu-operator
# Namespace:    monitoring
# Created on:   2025-12-28 02:18:48 +0000 UTC
# Labels:       <none>
# Annotations:  <none>
# Spec:
#   PodSelector:     app.kubernetes.io/name=prometheus
#   Not affecting ingress traffic
#   Allowing egress traffic:
#     To Port: 9400/TCP
#     To:
#       NamespaceSelector: kubernetes.io/metadata.name=gpu-operator
#   Policy Types: Egress
```

Create an ingress `NetworkPolicy` specification for the nvidia-dcgm-exporter `service` to allow access from the `monitoring` `namespace`:

```shell
cat <<'EOF' > network-policy-nvidia-dcgm-exporter-ingress-monitoring.yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: nvidia-dcgm-exporter-ingress-monitoring
  namespace: gpu-operator
spec:
  podSelector:
    matchLabels:
      app: nvidia-dcgm-exporter
  policyTypes:
  - Ingress
  ingress:
  - from:
    - namespaceSelector:
        matchLabels:
          kubernetes.io/metadata.name: monitoring
    ports:
    - protocol: TCP
      port: 9400
EOF
```

Apply the `NetworkPolicy` specification for the nvidia-dcgm-exporter `service`:

```shell
kubectl apply -f network-policy-nvidia-dcgm-exporter-ingress-monitoring.yaml

kubectl get networkpolicy -n gpu-operator
# NAME                             POD-SELECTOR                        AGE
# nvidia-dcgm-exporter-ingress-monitoring   app=nvidia-dcgm-exporter   14s

kubectl get pods -n gpu-operator -l app=nvidia-dcgm-exporter --show-labels
# NAME                         READY   STATUS    RESTARTS   AGE   LABELS
# nvidia-dcgm-exporter-rq6fz   1/1     Running   0          68m   app.kubernetes.io/managed-by=gpu-operator,app=nvidia-dcgm-exporter,controller-revision-hash=549c589f9b,helm.sh/chart=gpu-operator-v25.10.1,pod-template-generation=1

kubectl describe networkpolicy -n gpu-operator nvidia-dcgm-exporter-ingress-monitoring
# Name:         nvidia-dcgm-exporter-ingress-monitoring
# Namespace:    gpu-operator
# Created on:   2025-12-28 02:32:45 +0000 UTC
# Labels:       <none>
# Annotations:  <none>
# Spec:
#   PodSelector:     app=nvidia-dcgm-exporter
#   Allowing ingress traffic:
#     To Port: 9400/TCP
#     From:
#       NamespaceSelector: kubernetes.io/metadata.name=monitoring
#   Not affecting egress traffic
#   Policy Types: Ingress
```

## Grafana

Grafana is available, to get its admin user password, on the Controller node, run:

```shell
kubectl --namespace monitoring get secrets kube-prometheus-stack-grafana -o jsonpath="{.data.admin-password}" | base64 -d ; echo
# OR
kubectl --namespace monitoring get secret -l app.kubernetes.io/component=admin-secret -o jsonpath="{.items[0].data.admin-password}" | base64 --decode ; echo
# Lz...nd
```

To access Grafana, on the Controller node, run:

```shell
kubectl port-forward -n monitoring svc/kube-prometheus-stack-grafana 3000:80 --address 0.0.0.0
# Forwarding from 0.0.0.0:3000 -> 3000

curl http://localhost:3000
# <a href="/login">Found</a>.
```

You can now temporarily access the Grafana website from your local machine using the Controller’s IP via an SSH proxy:

- Cluster Controller
  - `http://11.11.1.1:3000/`

> Port forwarding is just for some quick tests while we don’t have the Grafana `service` exposed properly

### Expose Grafana

Expose the website of the Grafana `kube-prometheus-stack-grafana` deployment.

The service will have defaulted to run using the `ClusterIP` `NodeType` which means it won't be accessible outside the cluster: `kubectl describe svc -n monitoring kube-prometheus-stack-grafana`.
To expose the website, first create the `service` modification:

```shell
cat <<'EOF' > values_kube-prometheus-stack-grafana.yaml
grafana:
  service:
    type: NodePort
    nodePort: 32000 # Free port in the NodePort range
EOF
```

Then apply the modification:

```shell
helm upgrade kube-prometheus-stack prometheus-community/kube-prometheus-stack --namespace monitoring -f values_kube-prometheus-stack-grafana.yaml

kubectl --namespace monitoring get pods -l "release=kube-prometheus-stack"
# NAME                                                       READY   STATUS    RESTARTS   AGE
# kube-prometheus-stack-kube-state-metrics-59b9d4c6b-v56w5   1/1     Running   0          31m
# kube-prometheus-stack-operator-6bb65bd7b4-zb6f6            1/1     Running   0          28s
# kube-prometheus-stack-prometheus-node-exporter-bq2md       1/1     Running   0          31m
# kube-prometheus-stack-prometheus-node-exporter-h4wsh       1/1     Running   0          31m
# kube-prometheus-stack-prometheus-node-exporter-w7wkr       1/1     Running   0          31m

kubectl describe svc -n monitoring kube-prometheus-stack-grafana
# Type:                     NodePort
# ...
# NodePort:                 http-web  32000/TCP
# Endpoints:                10.244.1.16:3000
# ...

curl http://11.11.1.1:32000
# <a href="/login">Found</a>.
```

You can now access the Grafana website from your local machine, using the Controller’s IP, via an SSH proxy, without needing to start port forwarding on the controller.

### Nvidia DCGM Exporter Dashboard

To configure the dashboard:

- Grafana Dashboard / Dashboards / New / Import
  - `https://grafana.com/grafana/dashboards/12239-nvidia-dcgm-exporter-dashboard/`
    - Download the JSON since this `service` does not have access to the Internet
  - `https://github.com/NVIDIA/dcgm-exporter/blob/main/grafana/dcgm-exporter-dashboard.json`
    - This version of the dashboard may be more recent compared to the published version

Now run some GPU tests to see metrics in the dashboard, see [Tests](06_Tests.md).
