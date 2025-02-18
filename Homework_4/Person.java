import java.util.Random;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

public class Person implements Runnable {
    private Bathroom bathroom;
    private char gender;
    private int id;
    private Random random = new Random();

    public static final String ANSI_RESET = "\u001B[0m";
    public static final String ANSI_BLUE = "\u001B[34m";
    public static final String ANSI_PINK = "\u001B[35m";
    public static final String ANSI_YELLOW = "\u001B[33m";
    private static final DateTimeFormatter TIME_FORMATTER = DateTimeFormatter.ofPattern("HH:mm:ss");

    public Person(Bathroom bathroom, char gender, int id) {
        this.bathroom = bathroom;
        this.gender = gender;
        this.id = id;
    }

    @Override
    public void run() {
        try {
            while (true) {
                Thread.sleep((random.nextInt(20) + 1) * 1000);
                if (gender == 'M') {
                    System.err.println(String.format("%-52s", " ") + "|    " + ANSI_BLUE + "♂ Man ♂ " + ANSI_RESET + id + ANSI_YELLOW + " needs to use the bathroom " + ANSI_RESET + LocalTime.now().format(TIME_FORMATTER));
                } else {
                    System.err.println(String.format("%-52s", " ") + "|    " + ANSI_PINK + "♀ Woman ♀ " + ANSI_RESET + id + ANSI_YELLOW + " needs to use the bathroom " + ANSI_RESET + LocalTime.now().format(TIME_FORMATTER));
                }
                if (this.gender == 'M') bathroom.manEnter(this.id);
                else bathroom.womanEnter(this.id);
                Thread.sleep((random.nextInt(2) + 2) * 1000);
                if (this.gender == 'M') bathroom.manExit(this.id);
                else bathroom.womanExit(this.id);
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}