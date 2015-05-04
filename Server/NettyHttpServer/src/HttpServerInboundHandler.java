
import static io.netty.handler.codec.http.HttpHeaders.Names.CONNECTION;
import static io.netty.handler.codec.http.HttpHeaders.Names.CONTENT_LENGTH;
import static io.netty.handler.codec.http.HttpHeaders.Names.CONTENT_TYPE;
import static io.netty.handler.codec.http.HttpResponseStatus.OK;
import static io.netty.handler.codec.http.HttpResponseStatus.BAD_REQUEST;
import static io.netty.handler.codec.http.HttpVersion.HTTP_1_1;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.handler.codec.http.DefaultFullHttpResponse;
import io.netty.handler.codec.http.FullHttpResponse;
import io.netty.handler.codec.http.HttpContent;
import io.netty.handler.codec.http.HttpHeaders;
import io.netty.handler.codec.http.HttpHeaders.Values;
import io.netty.handler.codec.http.HttpRequest;
import io.netty.handler.codec.http.HttpResponseStatus;
import io.netty.util.CharsetUtil;

public class HttpServerInboundHandler extends ChannelInboundHandlerAdapter {

    private static Log log = LogFactory.getLog(HttpServerInboundHandler.class);

    private HttpRequest request;

    private ParsedUri parseUri(String uri) {
    	ParsedUri parsedUri = new ParsedUri();
    	String[] ss;
    	if (uri.contains("?") == false) {
        	parsedUri.path = uri;
        } else {
        	ss = uri.split("\\?");
        	parsedUri.path = ss[0];
        }
    	return parsedUri;
    }
    
    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg)
            throws Exception {
        if (msg instanceof HttpRequest) {
            request = (HttpRequest) msg;

            String uri = request.getUri();   
            ParsedUri parsedUri = parseUri(uri);

            System.out.println("Uri:" + uri);
            Caller caller = new Caller();
            CallInterface cb = HttpServer.pathHandlerMap.get(parsedUri.path);
            if (null == cb) {
            	sendError(ctx, BAD_REQUEST);
            	return;
            }
            caller.setCallfuc(cb, parsedUri);
            caller.call();
        }
        if (msg instanceof HttpContent) {
            HttpContent content = (HttpContent) msg;
            ByteBuf buf = content.content();
            System.out.println(buf.toString(io.netty.util.CharsetUtil.UTF_8));
            buf.release();

            String res = "I am OK";
            FullHttpResponse response = new DefaultFullHttpResponse(HTTP_1_1,
                    OK, Unpooled.wrappedBuffer(res.getBytes("UTF-8")));
            response.headers().set(CONTENT_TYPE, "text/plain");
            response.headers().set(CONTENT_LENGTH,
                    response.content().readableBytes());
            if (HttpHeaders.isKeepAlive(request)) {
                response.headers().set(CONNECTION, Values.KEEP_ALIVE);
            }
            ctx.write(response);
            ctx.flush();
        }
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) throws Exception {
        ctx.flush();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
        log.error(cause.getMessage());
        ctx.close();
    }
    
    private static void sendError(ChannelHandlerContext ctx,  
    		         HttpResponseStatus status) {  
	     FullHttpResponse response = new DefaultFullHttpResponse(HTTP_1_1,  
	         status, Unpooled.copiedBuffer("Failure: " + status.toString()  
	             + "\r\n", CharsetUtil.UTF_8));  
	     response.headers().set(CONTENT_TYPE,"text/plain; charset=UTF-8");  
	     ctx.writeAndFlush(response).addListener(ChannelFutureListener.CLOSE);
     }  

}