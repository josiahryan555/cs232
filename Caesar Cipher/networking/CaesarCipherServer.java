/* CaesarCipherServer.java connects to a command-line specified server and port, and allows user to communicate with it
 *
 * Usage: java CaesarCipherServer 9876
 *
 * Command line input: port number
 * Precondition: valid port number is passed
 * Use: This server will recieve a cipher number (between 1-25), send that same cipher number back to the client.  When this server recieves
 *    another input from the client, it will send back that string, but will add the cipher number to all of the alphabetic characters.  It
 *    wraps around, so z + 1 = a.  Upper case letters stay upper case (same for lower case), and only alphabetic characters are effects.
 * 
 * This Code was adapted and modified from code at : https://www.infoworld.com/article/2853780/socket-programming-for-scalable-systems.html?page=2
 * Author: Josiah Ryan
 * Date:         28/4/21
 */

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;

public class CaesarCipherServer extends Thread
{
    private  ServerSocket caesarCipherServer;
    private int port;
    private boolean running = false;

    public CaesarCipherServer( int port )
    {
        this.port = port;
    }

    public void startServer()
    {
        try
        {
            caesarCipherServer = new ServerSocket( port );
            this.start();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public void stopServer()
    {
        running = false;
        this.interrupt();
    }

    @Override
    public void run()
    {
        running = true;
        while( running )
        {
            try
            {
                System.out.println( "Listening for a connection" );

                // Call accept() to receive the next connection
                Socket socket = caesarCipherServer.accept();

                // Pass the socket to the RequestHandler thread for processing
                RequestHandler requestHandler = new RequestHandler( socket );
                requestHandler.start();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
        }
    }

    public static void main( String[] args )
    {
        if( args.length == 0 )  //too few arguments
        {
            System.out.println( "Usage: CaesarCipherServer <port>" );
            System.exit( 0 );
        }
        int port = Integer.parseInt( args[ 0 ] );
        System.out.println( "Start server on port: " + port );

        CaesarCipherServer server = new CaesarCipherServer( port );
        server.startServer();

        // Automatically shutdown in 1 minute
        try
        {
            Thread.sleep( 60000 );
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }

        server.stopServer();
    }
}

class RequestHandler extends Thread
{
    private Socket socket;
    RequestHandler( Socket socket )
    {
        this.socket = socket;
    }

    @Override
    public void run()
    {
        try
        {
            //when the server connects with a client, it prints off the time of the connection, and the IP address of the client
            Date date = new Date();
            System.out.println( "Received a connection at " + date.toString() + " from client at IP address: " + this.socket.getInetAddress().toString());

            // Get input and output streams
            BufferedReader in = new BufferedReader( new InputStreamReader( socket.getInputStream() ) );
            PrintWriter out = new PrintWriter( socket.getOutputStream() );

            // Write out our header to the client

            String line = in.readLine();

            try {
                int cipher = Integer.parseInt(line);  //turns passd cipher into an int
                if (cipher <= 1 || cipher > 25) {
                    //throw excpetion
                    throw new Exception();
                }

                out.println(line);
                out.flush();

                line = in.readLine(); //collects the empty line that the client passes
                out.println(line);    //ouputs the empty line to client (necissary to make this server work like the example server)

                // Echo lines back to the client until the client closes the connection or we receive an empty line
                out.flush();
                line = in.readLine();
                while( line != null && line.length() > 0 )
                {
                    //creates data holders that are used in the following code
                    StringBuilder encryptedString = new StringBuilder(line);
                    Character charAtIndex;
                    Character encryptedLetter;
                    int charIntAtIndex;
                    
                    //go through each letter in the client passed string, and add cipher to it (only modifies alphabetic characters)
                    for(int i = 0; i < line.length(); i++) {  //go through each letter in the passed string, and add cipher to it %
                        charAtIndex = encryptedString.charAt(i);
                        charIntAtIndex = (int) charAtIndex;

                        //this code adds cipher to the alphabetic characters (uses mod to wrap around if necissary)
                        if (Character.isUpperCase(charAtIndex)) {                         //handles uppercase character changes
                            if (Character.isUpperCase((char)(charIntAtIndex+cipher))) {   //if the char + cipher is still an upper case char
                                encryptedLetter = (char)(charAtIndex + cipher);           
                                encryptedString.setCharAt(i, encryptedLetter);            //it is set as the new char
                            } else {
                                //if not, this uses mod to wrap the character conversion around. so Z encrypted with cipher = 1 : A
                                encryptedLetter = (char) (65 + ((charIntAtIndex+cipher) % 91));
                                encryptedString.setCharAt(i, encryptedLetter);
                            }
                        } else if (Character.isLowerCase(charAtIndex)) {                  //handles lowercase character changes
                            if (Character.isLowerCase((char)(charIntAtIndex+cipher))) {
                                encryptedLetter = (char)(charAtIndex + cipher);
                                encryptedString.setCharAt(i, encryptedLetter);
                            } else {
                                //uses mod to wrap the character conversion around. so z encrypted with cipher = 1 : a
                                encryptedLetter = (char) (97 + ((charIntAtIndex+cipher) % 123));
                                encryptedString.setCharAt(i, encryptedLetter);
                            }
                        }
                    } //end for loop

                    out.println(encryptedString );  //sends encryptedString to the client
                    out.flush();
                    line = in.readLine();
                }
            }
            catch( Exception e )
            {
                out.println("ERROR : Invalid input: first value passed to this server must be an integer (between 1-25).  Connection closed.");
                out.flush();

                //closes connection
                in.close();
                out.close();
                socket.close();
            
                System.out.println( "Socket closed" );
            }

            // Close our connection
            in.close();
            out.close();
            socket.close();

            System.out.println( "Connection closed" );
        }   

    catch( Exception e )
        {
            System.out.println( "Socket closed" );
        }
    }
}