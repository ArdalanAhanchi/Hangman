import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class HangmanClient {
    public static void main(String[] args) throws Exception {
        if (args.length != 1) {
            System.err.println("Pass the server IP as the sole command line argument");
            return;
        }
        try (Socket socket = new Socket(args[0], 59123)) {
            System.out.println("Enter a username: ");
            Scanner scanner = new Scanner(System.in);
            String username = scanner.nextLine();
            System.out.println("Username: " + username);

            Scanner in = new Scanner(socket.getInputStream());
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

            System.out.println("Game has started. Type esc to quit.");
            System.out.println("Enter a character to guess: "); // if you type more than one only the first will be accepted

            while (scanner.hasNextLine()) {
                String guess = scanner.nextLine();
                out.println(guess);
                if (guess.equals("esc")) {
                    System.out.println("quitting game");
                    break;
                } else {
                    String serverOutput = in.nextLine();
                    if (serverOutput.substring(0,23).equals("Your guess was correct.")) {
                        System.out.println(serverOutput + "\nDisconnecting from server");
                        break;
                    } else {
                        System.out.println(serverOutput);
                    }
                }
            }
        }
    }
}