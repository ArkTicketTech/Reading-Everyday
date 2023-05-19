### **Eventual consistency**

— ****the *replication lag*

If an application reads from an asynchronous follower, it may see outdated information if the follower has fallen behind. This can lead to inconsistencies in the database, but eventually, the followers will catch up and become consistent with the leader. 

However, if the system is under load or network issues arise, the lag can increase significantly, leading to real problems for applications.

### Read-Your-Writes Consistency

Many applications allow users to submit data and view what they have submitted. When new data is submitted, it must be sent to the leader, but when the user views the data, the data can be from a follower.

Asynchronous replication can cause a problem where new data may not have reached the replica when the user views it shortly after making a write. Read-after-write consistency, also known as read-your-writes consistency, is needed in this situation.

- Guarantees that if the user reloads the page, they will always see any updates they submitted themselves.
- It does not promise visibility of other users' updates until some later time, but it reassures the user that their own input has been saved correctly.
