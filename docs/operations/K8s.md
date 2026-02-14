# Operations - K8s

## Shell Aliases

Convenient shell aliases:

```shell
cat <<'EOF' | tee -a .bash_aliases
alias kc='kubectl'

alias kcn='kubectl get nodes -o wide'

alias kcp='kubectl get pods --all-namespaces -o wide'
alias kcpd='kubectl describe pod'
alias kcpl='kubectl logs'
alias kcpdel='kubectl delete pod'

alias kcc='kubectl get configmaps'
alias kccd='kubectl get configmap -o yaml'
alias kccdel='kubectl delete configmap'
EOF
```

## Helm

```shell
helm repo list

helm list -A

helm get values kube-prometheus-stack -n monitoring -o yaml
```

## Clusters

```shell
kubectl config view

kubectl config get-clusters

kubectl cluster-info [dump]

kubectl get clusterpolicies -o wide
kubectl get clusterpolicy cluster-policy -o wide
kubectl describe clusterpolicy cluster-policy
```

## Services

```shell
kubectl get services
# NAME         TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)   AGE
# kubernetes   ClusterIP   10.96.0.1    <none>        443/TCP   108m

kubectl get svc --all-namespaces
# NAMESPACE       NAME                                             TYPE        CLUSTER-IP       EXTERNAL-IP   PORT(S)                        AGE
# default         kubernetes                                       ClusterIP   10.96.0.1        <none>        443/TCP                        120m
# gpu-operator    gpu-operator                                     ClusterIP   10.107.165.5     <none>        8080/TCP                       87m
# gpu-operator    nvidia-dcgm-exporter                             ClusterIP   10.96.162.95     <none>        9400/TCP                       87m
# kai-scheduler   admission                                        ClusterIP   10.111.198.243   <none>        443/TCP,8080/TCP               97m
# kai-scheduler   binder                                           ClusterIP   10.97.200.42     <none>        8080/TCP                       97m
# kai-scheduler   kai-scheduler-default                            ClusterIP   None             <none>        8080/TCP                       97m
# kai-scheduler   podgroup-controller                              ClusterIP   10.97.224.172    <none>        443/TCP                        97m
# kai-scheduler   queue-controller                                 ClusterIP   10.111.196.59    <none>        443/TCP,8080/TCP               97m
# kube-system     kube-dns                                         ClusterIP   10.96.0.10       <none>        53/UDP,53/TCP,9153/TCP         120m
# kube-system     kube-prometheus-stack-coredns                    ClusterIP   None             <none>        9153/TCP                       34m
# kube-system     kube-prometheus-stack-kube-controller-manager    ClusterIP   None             <none>        10257/TCP                      34m
# kube-system     kube-prometheus-stack-kube-etcd                  ClusterIP   None             <none>        2381/TCP                       34m
# kube-system     kube-prometheus-stack-kube-proxy                 ClusterIP   None             <none>        10249/TCP                      34m
# kube-system     kube-prometheus-stack-kube-scheduler             ClusterIP   None             <none>        10259/TCP                      34m
# kube-system     kube-prometheus-stack-kubelet                    ClusterIP   None             <none>        10250/TCP,10255/TCP,4194/TCP   33m
# monitoring      alertmanager-operated                            ClusterIP   None             <none>        9093/TCP,9094/TCP,9094/UDP     33m
# monitoring      kube-prometheus-stack-alertmanager               ClusterIP   10.111.235.145   <none>        9093/TCP,8080/TCP              34m
# monitoring      kube-prometheus-stack-grafana                    NodePort    10.106.154.62    <none>        80:32000/TCP                   34m
# monitoring      kube-prometheus-stack-kube-state-metrics         ClusterIP   10.106.148.62    <none>        8080/TCP                       34m
# monitoring      kube-prometheus-stack-operator                   ClusterIP   10.101.180.189   <none>        443/TCP                        34m
# monitoring      kube-prometheus-stack-prometheus                 ClusterIP   10.99.26.108     <none>        9090/TCP,8080/TCP              34m
# monitoring      kube-prometheus-stack-prometheus-node-exporter   ClusterIP   10.98.103.76     <none>        9100/TCP                       34m
# monitoring      prometheus-operated                              ClusterIP   None             <none>        9090/TCP                       33m

kubectl get svc -n gpu-operator
# NAME                   TYPE        CLUSTER-IP     EXTERNAL-IP   PORT(S)    AGE
# gpu-operator           ClusterIP   10.107.165.5   <none>        8080/TCP   76m
# nvidia-dcgm-exporter   ClusterIP   10.96.162.95   <none>        9400/TCP   76m

kubectl get svc -n monitoring
# NAME                                             TYPE        CLUSTER-IP       EXTERNAL-IP   PORT(S)                      AGE
# alertmanager-operated                            ClusterIP   None             <none>        9093/TCP,9094/TCP,9094/UDP   34m
# kube-prometheus-stack-alertmanager               ClusterIP   10.111.235.145   <none>        9093/TCP,8080/TCP            34m
# kube-prometheus-stack-grafana                    NodePort    10.106.154.62    <none>        80:32000/TCP                 34m
# kube-prometheus-stack-kube-state-metrics         ClusterIP   10.106.148.62    <none>        8080/TCP                     34m
# kube-prometheus-stack-operator                   ClusterIP   10.101.180.189   <none>        443/TCP                      34m
# kube-prometheus-stack-prometheus                 ClusterIP   10.99.26.108     <none>        9090/TCP,8080/TCP            34m
# kube-prometheus-stack-prometheus-node-exporter   ClusterIP   10.98.103.76     <none>        9100/TCP                     34m
# prometheus-operated                              ClusterIP   None             <none>        9090/TCP                     34m

kubectl describe svc -n monitoring kube-prometheus-stack-prometheus

kubectl describe svc -n monitoring kube-prometheus-stack-grafana
kubectl port-forward -n monitoring svc/kube-prometheus-stack-grafana 3000:80 --address 0.0.0.0
```

## Deployments

```shell
kubectl get deployments -n monitoring
# NAME                                       READY   UP-TO-DATE   AVAILABLE   AGE
# kube-prometheus-stack-grafana              1/1     1            1           23m
# kube-prometheus-stack-kube-state-metrics   1/1     1            1           23m
# kube-prometheus-stack-operator             1/1     1            1           23m
kubectl describe deployment kube-prometheus-stack-grafana -n monitoring

kubectl get deployments -n gpu-operator
# NAME                                                READY   UP-TO-DATE   AVAILABLE   AGE
# gpu-operator                                        1/1     1            1           77m
# nvidia-gpu-operator-node-feature-discovery-gc       1/1     1            1           77m
# nvidia-gpu-operator-node-feature-discovery-master   1/1     1            1           77m
kubectl describe deployment gpu-operator -n gpu-operator
```

## Stateful Sets

```shell
kubectl get statefulset -n monitoring
# NAME                                              READY   AGE
# alertmanager-kube-prometheus-stack-alertmanager   1/1     24m
# prometheus-kube-prometheus-stack-prometheus       1/1     24m

kubectl rollout restart statefulset prometheus-kube-prometheus-stack-prometheus -n monitoring
```

## Nodes

```shell
kubectl get nodes -o wide
```

## Queues

### KAI Scheduler

```shell
kubectl get queues -o wide

kubectl describe queue <queue name>

kubectl get pods -l kai.scheduler/queue=team-a --show-labels -o wide
```

### Kueue

```shell
kubectl get resourceflavors --all-namespaces -o wide
kubectl delete resourceflavors ...

kubectl get clusterqueues --all-namespaces -o wide
kubectl delete clusterqueues ...

kubectl get localqueues --all-namespaces -o wide
kubectl delete localqueues ...

kubectl -n default get workloads --all-namespaces -o wide
kubectl -n default delete workloads ...

kubectl -n default get jobs --all-namespaces -o wide
kubectl -n default delete jobs ...
```

## Pods

```shell
kubectl get pods --all-namespaces --show-labels -o wide

kubectl get pods -n <name space> -l <a label> -o yaml | grep labels -A 20
# E.g.
kubectl get pods -n gpu-operator -l app=nvidia-dcgm-exporter -o yaml | grep labels -A 20

kubectl get pod <pod name> -n <name space> -o jsonpath='{.spec.containers[*].ports[*]}'
kubectl get pod <pod name> -n <name space> -o yaml
kubectl exec <pod name> -n <name space> -- netstat -tulpn | grep LISTEN

kubectl describe pod <pod name>

kubectl delete pod <pod name>
```

## Logs

```shell
kubectl get pods --all-namespaces -o wide
kubectl logs -n kube-flannel kube-flannel-ds-5j5ch

kubectl get deployments -n monitoring
kubectl logs -n monitoring deployment/kube-prometheus-stack-operator
```

## ConfigMaps

```shell
kubectl get configmaps

kubectl get configmap -o yaml <configmap name>

kubectl delete configmap <configmap name>
```

## ServiceMonitors

```shell
kubectl get servicemonitors --all-namespaces
# NAMESPACE      NAME                                             AGE
# gpu-operator   nvidia-dcgm-exporter                             27m
# monitoring     kube-prometheus-stack-alertmanager               37m
# monitoring     kube-prometheus-stack-apiserver                  37m
# monitoring     kube-prometheus-stack-coredns                    37m
# monitoring     kube-prometheus-stack-grafana                    37m
# monitoring     kube-prometheus-stack-kube-controller-manager    37m
# monitoring     kube-prometheus-stack-kube-etcd                  37m
# monitoring     kube-prometheus-stack-kube-proxy                 37m
# monitoring     kube-prometheus-stack-kube-scheduler             37m
# monitoring     kube-prometheus-stack-kube-state-metrics         37m
# monitoring     kube-prometheus-stack-kubelet                    37m
# monitoring     kube-prometheus-stack-operator                   37m
# monitoring     kube-prometheus-stack-prometheus                 37m
# monitoring     kube-prometheus-stack-prometheus-node-exporter   37m

kubectl describe servicemonitor nvidia-dcgm-exporter -n gpu-operator

kubectl delete servicemonitors -n <name space> <servicemonitor name>
```
