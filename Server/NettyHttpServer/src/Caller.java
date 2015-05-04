
public class Caller  
{  
    private CallInterface caller;  
    private ParsedUri parsedUri;
  
    public void setCallfuc(CallInterface caller, ParsedUri parsedUri)  
    {  
       this.caller= caller;  
       this.parsedUri = parsedUri;
    }  
  
    public void call(){  
       this.caller.method(parsedUri);  
    }  
}    