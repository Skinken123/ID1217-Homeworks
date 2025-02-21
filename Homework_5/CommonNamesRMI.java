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

// RMI Interface
interface CommonNamesFinder extends Remote {
    boolean checkName(String name) throws RemoteException;
    void notifyMatch(String name) throws RemoteException;
    void done() throws RemoteException;
}

// Abstract Class for Common Functionality
abstract class AbstractProcess extends UnicastRemoteObject implements CommonNamesFinder {
    protected List<String> localList;
    protected Set<String> confirmedCommonNames = new HashSet<>();
    protected CommonNamesFinder[] peers;
    protected String[] peerNames;  // Store names of compared processes
    protected int doneSignals = 0;

    protected AbstractProcess(List<String> list) throws RemoteException {
        this.localList = list;
    }

    public synchronized boolean checkName(String name) throws RemoteException {
        return localList.contains(name);
    }

    public synchronized void notifyMatch(String name) throws RemoteException {
        confirmedCommonNames.add(name);
    }

    public synchronized void done() throws RemoteException {
        doneSignals++;
        if (doneSignals == 2) printCommonNames();
    }

    protected void printCommonNames() {
        System.out.println("Common names found by " + getProcessName() + 
                           " compared to processes" + Arrays.toString(peerNames) + ":");
        confirmedCommonNames.forEach(System.out::println);
    }

    protected abstract String getProcessName();

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
        for (CommonNamesFinder peer : peers) peer.done();  
    }
}

// Main Class to Launch Processes
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

    private static void runProcess(String name, List<String> data, int port) throws Exception {
        LocateRegistry.createRegistry(port);
        AbstractProcess process = new AbstractProcess(data) {
            protected String getProcessName() {
                return "Process " + name;
            }
        };
        Naming.rebind("//localhost:" + port + "/" + name, process);
    
        // Wait for peers to be available
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
    
                process.peerNames = Arrays.stream(new String[]{"F", "G", "H"})
                        .filter(n -> !n.equals(name))
                        .toArray(String[]::new);
    
                if (process.peers.length == 2) break;
                Thread.sleep(1000);
            } catch (Exception e) {
                Thread.sleep(1000);
            }
        }
        process.findCommonNames();
    }    
}
