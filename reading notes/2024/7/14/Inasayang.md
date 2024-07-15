# Learning OpenTelemetry Setting Up and Operating a Modern Observability System pp. 92-99

## ch7.Observing Infrastructure

基础设施可观察性关注两方面

- 基础设施提供者
  - 提供者是基础设施的实际“来源”，如数据中心或云提供商。Amazon Web Services (AWS)、Google Cloud Platform (GCP) 和 Microsoft Azure 都是基础设施提供者
- 基础设施平台
  - 平台是这些提供者之上的更高级别的抽象，提供某种托管服务，大小、复杂性和用途各不相同。Kubernetes 是容器编排的平台。FaaS平台，如 AWS Lambda、Google App Engine 和 Azure Cloud Functions，是无服务器平台。平台不一定仅限于代码或容器运行时；CI/CD平台，如 Jenkins，也是基础设施平台的一种
