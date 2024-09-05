

Camel empowers you to define routing and mediation rules in a variety of domain-specific languages (DSL, such as Java, XML, Groovy, Kotlin, and YAML). This means you get smart completion of routing rules in your IDE, whether in a Java or XML editor.


XML DSL of Route files from `home/customers/new` to `home/customers/old` folder

```xml
<route>
  <from uri = "file:home/customers/new"/>
  .... mediation rules
  <to uri = "file:home/customers/old" />
</route>
```



