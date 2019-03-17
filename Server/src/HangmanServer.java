import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;

/**
 * A new thread is started when client connects to port. Receiving, checking
 * and sending the response back is done on the thread.
 */
public class HangmanServer {

    /**
     * Server side. When client connects, a new thread is created and server continues
     * to listen for new connections. The application is limited by the thread pool.
     */
    public static void main(String[] args) throws Exception {
        try (ServerSocket listener = new ServerSocket(59123)) {
            System.out.println("Hangman server is running...");
            ExecutorService pool = Executors.newFixedThreadPool(10);
            while (true) {
                pool.execute(new Hangman(listener.accept()));
            }
        }
    }

    private static class Hangman implements Runnable {
        private Socket socket;
        private StringBuilder guesses = new StringBuilder(); //50 guesses
        private String wordToGuess = "test";
        Hangman(Socket socket) {
            this.socket = socket;
            // check if letter was guessed?
            // create new game
        }

        public void startGame() {
            System.out.println("Welcome to Hangman");

//            switch() {
//
//            }
        }
        public void endGame() {
            try {
                socket.close();
            } catch (IOException e) {
            }
            System.out.println("Closed: " + socket);
        }

        @Override
        public void run() {
            System.out.println("Connected: " + socket);
            try {
                Scanner in = new Scanner(socket.getInputStream());
                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                while (in.hasNextLine()) {
                    String input = in.nextLine().toLowerCase();
                    if (wordToGuess.equals(input)) {
                        out.println("Your guess was correct. The word: " + wordToGuess);
                        socket.close();
                        break;
                    }
                    if (guesses.toString().contains(input.substring(0,1))) {
                        out.println("You guessed: " + input.substring(0,1) + " before");
                    } else {
                        out.println("Your new guess has been checked: " + input.substring(0,1));
                        guesses.append(input.substring(0,1));
                    }
                }
            } catch (Exception e) {
                System.out.println("Error:" + socket);
            } finally {
                try {
                    socket.close();
                } catch (IOException e) {
                }
                System.out.println("Closed: " + socket);
            }
        }
    }
}