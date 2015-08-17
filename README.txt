Allowed Command:

//an analyst gives an estimate value
point <analyst_name> val

//from now on, alert this client everytime a new analyst joins and gives an estimate
subscribe <client_name> initiate

//define consensus as an array of each analyst's most recent estimate 
//after subscribing, alert the client when the min/max/median/mean/sample standard deviation changes beyond certain amount of percentage or value
subscribe <client_name> change max percentage
subscribe <client_name> change min percentage 
subscribe <client_name> change median percentage
subscribe <client_name> change mean percentage
subscribe <client_name> change stddev percentage 

//after subscribing, alert the client when the new given estimate is beyond the num*standard deviation from mean of consensus
subscribe <client_name> outlier num

//unsubscribe
unsubscribe <client_name> unsubscribe all events for that client