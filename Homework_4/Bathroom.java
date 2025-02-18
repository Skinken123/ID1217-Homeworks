import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

public class Bathroom {
    private final Object lock = new Object();
    private int menWaiting = 0, womenWaiting = 0;
    private int menInBathroom = 0, womenInBathroom = 0;
    private BathroomState bathroomState = null;

    public static final String ANSI_RESET = "\u001B[0m";
    public static final String ANSI_BLUE = "\u001B[34m";
    public static final String ANSI_PINK = "\u001B[35m";
    public static final String ANSI_GREEN = "\u001B[32m";
    public static final String ANSI_RED = "\u001B[31m";

    private static final DateTimeFormatter TIME_FORMATTER = DateTimeFormatter.ofPattern("HH:mm:ss");

    public enum BathroomState {
        MenEntering,
        MenLeaving,
        WomenEntering,
        WomenLeaving
    }

    public void manEnter(int threadId) throws InterruptedException {
        synchronized (lock) {
            menWaiting++;
            while (womenInBathroom > 0 || bathroomState == BathroomState.WomenEntering || (bathroomState == BathroomState.MenLeaving && womenWaiting > 0)) {
                lock.wait();
            }
            
            menWaiting--;
            if (bathroomState != BathroomState.MenLeaving)
                bathroomState = BathroomState.MenEntering;
            
            menInBathroom++;
            System.out.println(ANSI_BLUE + "♂ Man ♂ " + ANSI_RESET + threadId + ANSI_GREEN + " entered" + ANSI_RESET + " the bathroom at time " + LocalTime.now().format(TIME_FORMATTER) + "     |");
        }
    }

    public void manExit(int threadId) {
        synchronized (lock) {
            bathroomState = BathroomState.MenLeaving;
            menInBathroom--;
            
            System.out.println(ANSI_BLUE + "♂ Man ♂ " + ANSI_RESET + threadId + ANSI_RED + " left" + ANSI_RESET + " the bathroom at time " + LocalTime.now().format(TIME_FORMATTER) + "        |");

            if (menInBathroom == 0) {
                if (womenWaiting > 0)
                    bathroomState = BathroomState.WomenEntering;
                else
                    bathroomState = null;
            }
            lock.notifyAll();
        }
    }

    public void womanEnter(int threadId) throws InterruptedException {
        synchronized (lock) {
            womenWaiting++;
            while (menInBathroom > 0 || bathroomState == BathroomState.MenEntering || (bathroomState == BathroomState.WomenLeaving && menWaiting > 0)) {
                lock.wait();
            }

            womenWaiting--;
            if (bathroomState != BathroomState.WomenLeaving)
                bathroomState = BathroomState.WomenEntering;
            
            womenInBathroom++;
            System.out.println(ANSI_PINK + "♀ Woman ♀ " + ANSI_RESET + threadId + ANSI_GREEN + " entered" + ANSI_RESET + " the bathroom at time " + LocalTime.now().format(TIME_FORMATTER)+ "   |");
        }
    }

    public void womanExit(int threadId) {
        synchronized (lock) {
            bathroomState = BathroomState.WomenLeaving;
            womenInBathroom--;
            
            System.out.println(ANSI_PINK + "♀ Woman ♀ " + ANSI_RESET + threadId + ANSI_RED + " left" + ANSI_RESET + " the bathroom at time " + LocalTime.now().format(TIME_FORMATTER) + "      |");

            if (womenInBathroom == 0) {
                if (menWaiting > 0)
                    bathroomState = BathroomState.MenEntering;
                else
                    bathroomState = null;
            }
            lock.notifyAll();
        }
    }
}