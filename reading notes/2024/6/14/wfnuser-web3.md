struct goal {
 uint id;
 string name;
 address owner;
 
 string type;

 uint requiredStake;
 bool precisely; 

 uint state; // 任务状态 - 0 未开始 1 开始 2 完成 ...
 date created;
 date begin;
 date end;

 address[] participants;
 
 tasks; // TBD - 数字就好还是要结构体 8 = 8 stage
 
 // 每个 p 对 每个 t 的完成情况记录
 // progress - 01001100

 mapping(address => uint) progress; // 可以考虑用 bitmap 表达任务完成情况 否则需要二维数组 - 对于循环任务可以记录当前天数
 mapping(address => bool) isParticipant;
 mapping(address => bool) isClaimed;

 // 保留每个 p 清算后的结果
 mapping(address => uint) rewards;
}