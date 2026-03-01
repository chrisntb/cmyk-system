# Managment Services - Setup

This is WIP.

## Postgres Operator - CloudNativePG

```shell
ansible-playbook -i inventory/dev/hosts.yaml playbooks/setup/13_cloud-native-pg_plays.yaml
```

## Gateway API

```shell
# See https://github.com/kubernetes-sigs/gateway-api/releases
kubectl apply --server-side -f https://github.com/kubernetes-sigs/gateway-api/releases/download/v1.4.1/experimental-install.yaml
# "Warning: Regarding the Experimental CRDs
#   - please note that the experimental CRDs for this release
#     are too large for a standard kubectl apply.
#     You may receive an error like metadata.annotations:
#       Too long: may not be more than 262144 bytes"

helm repo add haproxy-ingress https://haproxytech.github.io/helm-charts
helm repo update
helm install haproxy-ingress haproxy-ingress/haproxy-ingress \
  --namespace gateway \
  --create-namespace \
  --set gateways.enabled=true \
  --set controller.service.type=LoadBalancer

# This deploys the controller with GatewayClass haproxy ready,
#   provisions a LoadBalancer Service, and enables Gateway API parsing
kubectl get pods,svc -n gateway
kubectl get gatewayclass haproxy
```
