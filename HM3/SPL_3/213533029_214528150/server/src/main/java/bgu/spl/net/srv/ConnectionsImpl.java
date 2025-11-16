package bgu.spl.net.srv;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T> {

    ConcurrentHashMap<Integer, ConnectionHandler<T>> idMapHandler;

    public ConnectionsImpl() {

        idMapHandler = new ConcurrentHashMap<>();
    }

    @Override
    public void connect(int connectionId, ConnectionHandler<T> handler) {
        // TODO Auto-generated method stub
        idMapHandler.putIfAbsent((Integer)connectionId, handler);
    }

    // @SuppressWarnings("unchecked")
    @Override
    public boolean send(int connectionId, T msg) {
        // TODO Auto-generated method stub
        if (idMapHandler.containsKey((Integer)connectionId)) {
            idMapHandler.get(connectionId).send(msg);
            return true;
        }
        return false;

    }

    @Override
    public void disconnect(int connectionId) {
        // TODO Auto-generated method stub
        if (idMapHandler.contains(connectionId)) {
            try {
                idMapHandler.get((Integer) connectionId).close();
                idMapHandler.remove(connectionId);

            } catch (IOException ignored) {
            }

        }
    }
}
