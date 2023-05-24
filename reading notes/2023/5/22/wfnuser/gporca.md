不同执行任务之间是如何彼此调用的？
通过调度器以任务为粒度彼此调用；在执行某个任务的时候会 schedule 其他类型的任务；每个任务执行的时候都是在执行一个状态机；第一个任务是根节点所在组的 GroupOptimization
任务是以组为单位进行的
每个 group 有以下几种状态：
``` cpp
		  	// states of a group
		  	enum EState
		  	{
		  		estUnexplored,	// initial state
		  
		  		estExploring,  // ongoing exploration
		  		estExplored,   // done exploring
		  
		  		estImplementing,  // ongoing implementation
		  		estImplemented,	  // done implementing
		  
		  		estOptimizing,	// ongoing optimization
		  		estOptimized,	// done optimizing
		  
		  		estSentinel
		  	};
		  
```
被 suspend 的任务什么时候有机会再次被调度？
schedule 子任务的时候会把父任务信息记录上，比如 `CJobGroupExpressionExploration::ScheduleChildGroupsJobs` 中，调用 `CJobGroupExploration::ScheduleJob` 真的加入子任务的时候，会 `psc->Psched()->Add(pjge, pjParent);`
`Complete(pj);` 完成job的时候，则会调用 ` ResumeParent(pj);`；从而让父任务再次进入 `m_listjlWaiting`