import java.time.LocalTime;

public class Bathroom {
    //Block synchronization
    private final Object bathroom = new Object();
    private final Object menWait = new Object();
    private final Object womanWait = new Object();

    public static final String ANSI_RESET = "\u001B[0m";
    public static final String ANSI_BLUE = "\u001B[34m";
    public static final String ANSI_PINK = "\u001B[35m";
    public static final String ANSI_GREEN = "\u001B[32m";
    public static final String ANSI_RED = "\u001B[31m";

    private int nrOfWaitingMen = 0, nrOfWaitingWomen = 0, nrOfPeopleInBathroom = 0;
    //private char currentGender = 'N'; // 'M' for men, 'W' for women, 'N' for none 
 
    public void manEnter(int threadId) throws InterruptedException {
        synchronized (menWait) {
            if (nrOfPeopleInBathroom > 0) {
                nrOfWaitingMen++;
                menWait.wait();
            }
        }
        synchronized (bathroom) {
            nrOfPeopleInBathroom++;
            System.out.println(ANSI_BLUE + "Man " + ANSI_RESET + threadId + ANSI_GREEN + " entered" + ANSI_RESET + " the bathroom at time " + LocalTime.now() + "\n");
            synchronized (menWait) {
                if (nrOfWaitingMen > 0 && nrOfWaitingWomen < 2) { // maybe if there are more than 2 women waiting they should be let in
                    nrOfWaitingMen--;
                    menWait.notify();
                } else if (nrOfWaitingMen > 0) {
                    nrOfWaitingMen--;
                }
            }
        }
    }

    public void manExit(int threadId) throws InterruptedException {
        synchronized (bathroom) {
            nrOfPeopleInBathroom--;
            System.out.println(ANSI_BLUE + "Man " + ANSI_RESET + threadId + ANSI_RED + " left" + ANSI_RESET + " the bathroom at time " + LocalTime.now() + "\n");
            if (nrOfPeopleInBathroom == 0){
                synchronized(womanWait) {
                    if (nrOfWaitingWomen > 0) {
                        nrOfWaitingWomen--;
                        womanWait.notify();
                    } else if (nrOfWaitingMen > 0) {
                        synchronized (menWait) {
                            nrOfWaitingMen--;
                            menWait.notify();
                        }
                    }
                }
            }
        }
    }

    public void womanEnter(int threadId) throws InterruptedException {
        synchronized (womanWait) {
            if (nrOfPeopleInBathroom > 0) {
                nrOfWaitingWomen++;
                womanWait.wait();
            }
        }
        synchronized (bathroom) {
            nrOfPeopleInBathroom++;
            System.out.println(ANSI_PINK + "Woman " + ANSI_RESET + threadId + ANSI_GREEN + " entered" + ANSI_RESET + " the bathroom at time " + LocalTime.now() + "\n");
            synchronized (womanWait) {
                if (nrOfWaitingWomen > 0 && nrOfWaitingMen < 2) { // maybe if there are more than 2 women waiting they should be let in
                    nrOfWaitingWomen--;
                    womanWait.notify();
                } else if (nrOfWaitingWomen > 0) {
                    nrOfWaitingWomen--;
                }
            }
        }
    }

    public void womanExit(int threadId) throws InterruptedException {
        synchronized (bathroom) {
            nrOfPeopleInBathroom--;
            System.out.println(ANSI_PINK + "Woman " + ANSI_RESET + threadId + ANSI_RED + " left" + ANSI_RESET + " the bathroom at time " + LocalTime.now() + "\n");
            if (nrOfPeopleInBathroom == 0){
                synchronized(menWait) {
                    if (nrOfWaitingMen > 0) {
                        nrOfWaitingMen--;
                        menWait.notify();
                    } else if (nrOfWaitingWomen > 0) {
                        synchronized (womanWait) {
                            nrOfWaitingWomen--;
                            womanWait.notify();
                        }
                    }
                }
            }
        }
    }
 }