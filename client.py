import tkinter
import connectToServer

width, height = 800, 800
canvas = tkinter.Canvas(width=width, height=height)
canvas.pack()


def get_server_ip():
    response = connectToServer.check_server_status()
    if not response:
        print("Server is not running.")
        return

    print(f"Server is running on IP: {response}")


connectButton = tkinter.Button(text="Connect", command=get_server_ip)
connectButton.pack()

tkinter.mainloop()

