/* CaesarCipherClient.java connects to a command-line specified server and port, and allows user to communicate with it
 *
 * Usage: java CaesarCipherClient brooks.cs.calvin.edu 9876
 *
 * Command line input: server name, port number
 * Precondition: server exists and port number is the number of a port used to connect with a client
 * Use: if a connection is established with a valid server like brooks.cs.calvin.edu (port 9876), this program will, given a cipher number n, send any 
 *    any user inputed string to the server, where each letter in the string will be shifted n positions.  This is implemented in a loop, and quit must be 
 *    called to exit the loop.
 * 
 * This Code was adapted and modified from code at : https://www.infoworld.com/article/2853780/socket-programming-for-scalable-systems.html
 * Author: Josiah Ryan
 * Date:         28/4/21
 */
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;
import java.net.SocketException;
import java.lang.Integer;
import java.util.Scanner;                   // convenient I/O

public class CaesarCipherClient
{
    public static void main( String[] args )
    {
        if( args.length < 2 )
        {
            System.out.println( "Usage: CaesarCipherClient <server> <path>" );
            System.exit( 0 );
        }
        //initializes the hostName and port variables so they can be used in the try catch block
        String hostName = "";  
        int port = 0;

        //will only work if the port passed is an integer
        try {
            hostName = args[ 0 ];
            port = java.lang.Integer.parseInt( args[ 1 ]);//args[ 1 ]);
        }
        catch (Exception e ) {  //catches error if user passed a non-integer for the port
            System.out.println("arguments entered must be a host name (string) and a port number (integer)");
            System.exit( 0 );
        }

        System.out.println("Enter the cipher number (number between 1-25): ");
        Scanner keyboard = new Scanner(System.in);
        int cipherNum = 0;  //initialized here, but is set in the try loop, if an excpetion is thrown, the program stops.  Unorthodox, but this stopped an error.

        try {
            cipherNum = Integer.parseInt(keyboard.nextLine());     //gets a the cipher number (integer) from the user to send to the server
            if (cipherNum <= 1 || cipherNum > 25) {
                //throw excpetion
                throw new Exception();
            }
        }
        catch (Exception e) {
            System.out.println("first argument must be a number between 1-25");
            System.exit( 0 );
        }
        String input = "";                                         //creates an input string to get an input from the user later

        try
        {
            // Connect to the server
            Socket socket = new Socket( hostName, port );


            // Create input and output streams to read from and write to the server
            PrintStream out = new PrintStream( socket.getOutputStream() );
            BufferedReader in = new BufferedReader( new InputStreamReader( socket.getInputStream() ) );

            out.println( cipherNum );     //prints the user entered cipherNum to the server
            out.println();                //prints empty line to the server
            String line = in.readLine();  //reads the server's response to the user
            System.out.println( line );   //prints the server's response to user(echoing the passed number)
            line = in.readLine();


            //get first input for the user input loop
            System.out.print("> ");
            input = keyboard.nextLine();     //gets a the cipher number (integer) from the user to send to the server

            //user input loop
            while (!input.equals("quit")) {

                out.println(input);             //send input to server
                line = in.readLine();           //store server's response
                System.out.println( line );     //print server's response

                System.out.print("> ");
                input = keyboard.nextLine();     //gets a the cipher number (integer) from the user to send to the server 

            }

            // Close our streams
            in.close();
            out.close();
            socket.close();
        }
        //catch( SocketException e )
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }
}