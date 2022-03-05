# Security HOWTO

## Injection Attacks

SpinoDB, like all query based database, can be subject to injection type attacks. 


### Example 1

Make sure numerical inputs are infact numerical. 

Flawed code example:
```
var input = body.idx; 
col.drop('{ idx: ' + input + '}');
```
By providing malicious input in the example above, it's possible for an attacker to delete the entire collection. For find queries, failure to sanitise numerical inputs makes it trivial for an attacker to retrieve the entire collection. 

An example of malicious input might be '{$gt: -1}'. This would cause all entries with field idx greater than -1 to be deleted. 

Safer implementation
```
var input = parseInt(body.idx);
if(isNaN(input) == true) {
    // deal with error
}
else {
    col.dropOne('{ idx: ' + input + '}');
}

```

### Example 2

Always escape string inputs. 

Flawed code example
```
var input = body.name;
var userProfile = col.findOne('{ name: "' + input + '"}');
```

A malicious input can corrupt the query and cause it to fail unexpectedly. Always use the escape function to properly format string inputs. 

Safer implementation
```
var input = spino.escape(body.name);
var userProfile = col.findOne('{ name: "' + input + '"}');
```


