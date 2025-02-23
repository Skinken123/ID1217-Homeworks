package Homework_5;

import java.rmi.*;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.registry.LocateRegistry;
import java.util.*;

/**
 * Combined RMI Program: Find common names across three distributed lists (F, G, H).
 * Usage:
 * 1. Compile: javac Homework_5/CommonNamesRMI.java
 * 3. Run each process in separate terminals:
 *    java Homework_5.CommonNamesRMI F
 *    java Homework_5.CommonNamesRMI G 
 *    java Homework_5.CommonNamesRMI H
 */

// RMI Interface defining remote methods for name comparison and coordination
interface CommonNamesFinder extends Remote {
    boolean checkName(String name) throws RemoteException; // Checks if a name exists locally
    void notifyMatch(String name) throws RemoteException;  // Notifies about a confirmed common name
    void done() throws RemoteException;                    // Signals completion of processing
}

// Abstract class providing shared functionality for processes F, G, and H
abstract class AbstractProcess extends UnicastRemoteObject implements CommonNamesFinder {
    protected List<String> localList;                      // List of local names for the process
    protected Set<String> confirmedCommonNames = new HashSet<>(); // Stores confirmed common names
    protected CommonNamesFinder[] peers;                   // References to peer processes
    protected String[] peerNames;                          // Store names of compared processes
    protected int doneSignals = 0;                         // Tracks completion signals from peers 

    // Constructor initializing the local list
    protected AbstractProcess(List<String> list) throws RemoteException {
        this.localList = list;
    }

    // Checks if the given name exists in the local list
    public synchronized boolean checkName(String name) throws RemoteException {
        return localList.contains(name);
    }

    // Adds a confirmed common name to the local set
    public synchronized void notifyMatch(String name) throws RemoteException {
        confirmedCommonNames.add(name);
    }

    // Increments the done signal count and prints common names when all peers are done
    public synchronized void done() throws RemoteException {
        doneSignals++;
        if (doneSignals == 2) printCommonNames();
    }

    // Prints the confirmed common names along with peer process names
    protected void printCommonNames() {
        System.out.println("Common names found by " + getProcessName() + 
                           " compared to processes" + Arrays.toString(peerNames) + ":");
        confirmedCommonNames.forEach(System.out::println);
    }

    // Abstract method to be implemented by concrete classes for process naming
    protected abstract String getProcessName();

    // Core logic for finding common names by comparing with peers
    protected void findCommonNames() throws RemoteException {
        for (String name : localList) {
            boolean inAll = true;
            for (CommonNamesFinder peer : peers) {
                if (!peer.checkName(name)) {
                    inAll = false;
                    break;
                }
            }
            if (inAll) {
                confirmedCommonNames.add(name);
                for (CommonNamesFinder peer : peers) peer.notifyMatch(name);
            }
        }
        for (CommonNamesFinder peer : peers) peer.done();  // Notify peers of completion
    }
}

// Main class responsible for launching RMI processes F, G, and H
public class CommonNamesRMI {

    public static void main(String[] args) throws Exception {
        if (args.length != 1) {
            System.out.println("Usage: java CommonNamesRMI [F|G|H]");
            return;
        }

        String processId = args[0];
        switch (processId) {
            case "F":
                runProcess("F", Arrays.asList("Alice", "Anders", "Bob", "Charlie", "David", "Isabella", "Vlad", "Jack", "Karen", "Alexander", "Leo", "Mia", "Noah", "Olivia", "Peter", "Quinn", "Ryan", "Gustav"), 1099);
                break;
            case "G":
                runProcess("G", Arrays.asList("Charlie", "Vlad", "David", "Eve", "Frank", "Samuel", "Tina", "Ursula", "Victor", "Anders", "Wendy", "Xavier", "Yasmine", "Zachary", "Liam", "Alexander", "Sophia", "Gustav"), 1100);
                break;
            case "H":
                runProcess("H", Arrays.asList("Charlie", "Alexander", "David", "George", "Hannah", "Isaac", "Julia", "Kevin", "Laura", "Michael", "Vlad", "Natalie", "Oliver", "Patricia", "Quentin", "Rachel", "Gustav", "Anders"), 1101);
                break;
            default:
                System.out.println("Invalid process ID. Use F, G, or H.");
        }
    }

    // Initializes and runs a process with specified data and port
    private static void runProcess(String name, List<String> data, int port) throws Exception {
        LocateRegistry.createRegistry(port);   // Create RMI registry on the specified port, a place for the server to register services it offers and a place for clients to query
        
        // Create an anonymous subclass of AbstractProcess with a custom getProcessName implementation
        AbstractProcess process = new AbstractProcess(data) {
            protected String getProcessName() {
                return "Process " + name;
            }
        };

        // Bind the process to the registry for remote access
        Naming.rebind("//localhost:" + port + "/" + name, process);
    
        // Wait until all peers (F, G, H) become available in the registry
        while (true) {
            try {
                process.peers = Arrays.stream(new String[]{"F", "G", "H"})
                        .filter(n -> !n.equals(name))
                        .map(n -> {
                            try {
                                int p = n.equals("F") ? 1099 : n.equals("G") ? 1100 : 1101;
                                return (CommonNamesFinder) Naming.lookup("//localhost:" + p + "/" + n);
                            } catch (Exception e) {
                                return null;
                            }
                        })
                        .filter(Objects::nonNull)
                        .toArray(CommonNamesFinder[]::new);
                
                // Store peer names for later reporting
                process.peerNames = Arrays.stream(new String[]{"F", "G", "H"})
                        .filter(n -> !n.equals(name))
                        .toArray(String[]::new);
    
                if (process.peers.length == 2) break;  // Exit loop when all peers are connected
                Thread.sleep(1000);  // Retry after 1 second if not all peers are available
            } catch (Exception e) {
                Thread.sleep(1000);  // Retry on exception
            }
        }
        process.findCommonNames();  // Start the process to find common names
    }    
}
