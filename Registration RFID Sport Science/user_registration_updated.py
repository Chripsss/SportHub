# Import necessary libraries
from tkinter import *
from tkinter import ttk
from tkinter import messagebox
import tkinter
import random
import pymysql
import csv
from datetime import datetime
import numpy as np
import serial

# Create SerialObject with COM port "COM20"
mySerial = serial.Serial("/dev/cu.usbmodem14201", 9600)

# Create the main window
window = tkinter.Tk()

# Set the window title
window.title("SportScience Registration Form")

# Create a Treeview widget to display data (headings only for now)
my_tree = ttk.Treeview(window, show='headings', height=20)

# Set the window size
window.geometry("720x640")
style=ttk.Style()

# Initialize an array to store placeholder values for registration form fields
placeholderArray = ['', '', '', '', '', '', '']
sequence_counter = 0
# Create StringVar objects for each placeholder value
for i in range(0, 7):
    placeholderArray[i] = tkinter.StringVar()

def connection():
    conn = pymysql.connect(
        host = '192.168.0.199',
        user = 'Tiara_21',
        password = 'password',
        db = 'registration'
    )
    return conn
conn = connection()
cursor = conn.cursor()


def setph(word,num):
    for ph in range(0,7):
        if ph == num:
            placeholderArray[ph].set(word)


# DRead data from mysql
def read():
    cursor.connection.ping()
    sql=f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`, `EntryDate` FROM `user` ORDER BY 'ID' DESC"
    cursor.execute(sql)
    result=cursor.fetchall()
    conn.commit()
    conn.close()
    return result

def readSpecific():
    UniqueID = str(IDEntry.get())
    UniqueUID = str(UIDEntry.get())
    UniquePlayerName = str(NameEntry.get())
    UniqueDoB = str(DoBEntry.get())
    Uniqueemail = str(emailEntry.get())
    UniqueWhatsApp = str(whatsAppEntry.get())
    UniqueMembershipType = str(membershipTypeCombo.get()) 

    cursor.connection.ping()
    if(UniqueID and UniqueID.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `ID` LIKE '{UniqueID}' "
    elif(UniqueUID and UniqueUID.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `UID` LIKE '{UniqueUID}' "
    elif(UniquePlayerName and UniquePlayerName.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `PlayerName` LIKE '{UniquePlayerName}' "
    elif(UniqueDoB and UniqueDoB.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `DoB` LIKE '{UniqueDoB}' "
    elif(Uniqueemail and Uniqueemail.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `Email` LIKE '{Uniqueemail}' "
    elif(UniqueWhatsApp and UniqueWhatsApp.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `WhatsApp` LIKE '{UniqueWhatsApp}' "
    elif(UniqueMembershipType and UniqueMembershipType.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `MembershipType` LIKE '{UniqueMembershipType}' "
    else:
        messagebox.showwarning("","Please fill up one of the entry")
        return
    cursor.execute(sql)
    result=cursor.fetchall()
    conn.commit()
    conn.close()
    return result


# Define a function to refresh the contents of the Treeview widget
def refreshTable():
    # Delete all existing data in the Treeview widget
    for data in my_tree.get_children():
        my_tree.delete(data)

    # Insert new data from dummydata into the Treeview widget
    for array in read():
        # The iid parameter represents the item identifier (a unique identifier for each row)
        my_tree.insert(parent='', index='end', iid=array, text="", values=(array), tag="orow")

    # Configure the tag 'orow' to set the background color of the rows
    my_tree.tag_configure('orow', background='#EEEEEE')

    # Pack the Treeview widget (assuming it's the main container for data display)
    my_tree.pack()

def refreshTableSpecific():
    # Delete all existing data in the Treeview widget
    for data in my_tree.get_children():
        my_tree.delete(data)

    # Insert new data from dummydata into the Treeview widget
    for array in readSpecific():
        # The iid parameter represents the item identifier (a unique identifier for each row)
        my_tree.insert(parent='', index='end', iid=array, text="", values=(array), tag="orow")

    # Configure the tag 'orow' to set the background color of the rows
    my_tree.tag_configure('orow', background='#EEEEEE')

    # Pack the Treeview widget (assuming it's the main container for data display)
    my_tree.pack()


def generateID():
    try:
        # Fetch the latest ID from the database
        cursor.connection.ping()
        sql = "SELECT `ID` FROM `user` ORDER BY `ID` DESC LIMIT 1"
        cursor.execute(sql)
        latest_id = cursor.fetchone()

        if latest_id:
            latest_id = str(latest_id[0])  # Convert to string
            # Extract the last 3 digits and increment
            last_digits = int(latest_id[-3:])
            sequence_counter = last_digits + 1
        else:
            # If no records exist, start from 1
            sequence_counter = 1

        current_time = datetime.now()
        year_part = str(current_time.year)[-2:]
        month_part = str(current_time.month).zfill(2)
        day_part = str(current_time.day).zfill(2)
        sequence_part = str(sequence_counter).zfill(3)
        UniqueID = f"{year_part}{month_part}{day_part}{sequence_part}"

        # Assign values directly to placeholderArray
        placeholderArray[0].set(UniqueID)
        # ... continue for other indices as needed

        print("generated: " + UniqueID)
    except Exception as e:
        print(e)
        messagebox.showwarning(" ", "Error while generating ID")



def read_RFID():
    try:
        uid = mySerial.readline().decode('utf-8').strip()
        placeholderArray[1].set(uid)
    except Exception as err:
        messagebox.showwarning(" ", "Error occured ref: "+str(err))

def save():
    ID = str(IDEntry.get())
    UID = str(UIDEntry.get())
    PlayerName = str(NameEntry.get())
    DoB = str(DoBEntry.get())
    email = str(emailEntry.get())
    WhatsApp = str(whatsAppEntry.get())
    MembershipType = str(membershipTypeCombo.get())
    
    valid = True
    
    if not (ID and ID.strip()) or not (UID and UID.strip()) or not (PlayerName and PlayerName.strip()) or not (DoB and DoB.strip()) or not (email and email.strip()) or not (WhatsApp and WhatsApp.strip()) or not (MembershipType and MembershipType.strip()):
        messagebox.showwarning(" ", "Please fill up all entries") 
        return

    try:
        cursor.connection.ping()
        
        # Use backticks (`) instead of single quotes (') for table and column names
        sql = f"SELECT * FROM `user` WHERE `ID` = '{ID}'"
        cursor.execute(sql)
        
        checkID = cursor.fetchall()
        
        if len(checkID) > 0:
            messagebox.showwarning("", "ID already used")
            return
        else:
            cursor.connection.ping()
            
            # Fix the syntax error in the INSERT statement
            sql = f"INSERT INTO `user` (`ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`) VALUES ('{ID}', '{UID}', '{PlayerName}', '{DoB}', '{email}', '{WhatsApp}', '{MembershipType}')"
            cursor.execute(sql)
            messagebox.showinfo("", "Inserted Successfully")
            conn.commit()
            conn.close()
            for num in range (0,7):
                setph('',(num))
    except Exception as e:
        print(e)  # Print the exception for debugging
        messagebox.showwarning(" ", "Error while saving")
        return
    refreshTable()
    

def update():
    selectedID = ''
    try:
        selectedItem = my_tree.selection()[0]
        selectedID = str(my_tree.item(selectedItem)['values'][0])
    except:
        messagebox.showwarning("", "Please select a data row")
    print(selectedID)
    ID = str(IDEntry.get())
    UID = str(UIDEntry.get())
    PlayerName = str(NameEntry.get())
    DoB = str(DoBEntry.get())
    email = str(emailEntry.get())
    WhatsApp = str(whatsAppEntry.get())
    MembershipType = str(membershipTypeCombo.get()) 
    if not (ID and ID.strip()) or not (UID and UID.strip()) or not (PlayerName and PlayerName.strip()) or not (DoB and DoB.strip()) or not (email and email.strip()) or not (WhatsApp and WhatsApp.strip()) or not (MembershipType and MembershipType.strip()):
        messagebox.showwarning(" ", "Please fill up all entries") 
        return
    if (selectedID!=ID):
        messagebox.showwarning("","You can't change registered ID")
        return 
    try:
        cursor.connection.ping()
        
        # Use backticks (`) instead of single quotes (') for table and column names
        sql = f"UPDATE `user` SET `UID` = '{UID}', `PlayerName` = '{PlayerName}', `DoB` = '{DoB}', `Email` = '{email}', `WhatsApp` = '{WhatsApp}', `MembershipType` = '{MembershipType}' WHERE `ID` = '{ID}' "
        cursor.execute(sql)
        messagebox.showinfo("", "Updated Successfully")
        conn.commit()
        conn.close()
        for num in range (0,7):
            setph('',(num))
    except Exception as err:
        messagebox.showwarning(" ", "Error occured ref: "+str(err))
        return
    refreshTable()


def delete():
    try:
        if(my_tree.selection()[0]):
            decision = messagebox.askquestion("", "Delete the selected data?")
            if(decision != 'yes'):
                return
            else:
                selectedItem = my_tree.selection()[0]
                ID = str(my_tree.item(selectedItem)['values'][0])
                try:
                    cursor.connection.ping()
                    sql = f"DELETE FROM `user` WHERE `ID` = '{ID}'"
                    cursor.execute(sql)
                    messagebox.showinfo("", "Deleted Successfully")
                    conn.commit()
                    conn.close()
                    for num in range (0,7):
                        setph('',(num))
                except Exception as err:
                    messagebox.showwarning(" ", "Error occured ref: "+str(err))
                    return
                refreshTable()
    except:
         messagebox.showwarning("", "Please select a data row")


def select():
    try:
        selectedItem = my_tree.selection()[0]
        ID = str(my_tree.item(selectedItem)['values'][0])
        UID = str(my_tree.item(selectedItem)['values'][1])
        PlayerName = str(my_tree.item(selectedItem)['values'][2])
        DoB = str(my_tree.item(selectedItem)['values'][3])
        email = str(my_tree.item(selectedItem)['values'][4])
        WhatsApp = str(my_tree.item(selectedItem)['values'][5])
        MembershipType = str(my_tree.item(selectedItem)['values'][6])
        setph(ID,0)
        setph(UID,1)
        setph(PlayerName,2)
        setph(DoB,3)
        setph(email,4)
        setph(WhatsApp,5)
        setph(MembershipType,6)
    except:
        messagebox.showwarning("","Please select a data row")


def find():
    UniqueID = str(IDEntry.get())
    UniqueUID = str(UIDEntry.get())
    UniquePlayerName = str(NameEntry.get())
    UniqueDoB = str(DoBEntry.get())
    Uniqueemail = str(emailEntry.get())
    UniqueWhatsApp = str(whatsAppEntry.get())
    UniqueMembershipType = str(membershipTypeCombo.get()) 
    refreshTableSpecific()

    cursor.connection.ping()
    if(UniqueID and UniqueID.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `ID` LIKE '{UniqueID}' "
    elif(UniqueUID and UniqueUID.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `UID` LIKE '{UniqueUID}' "
    elif(UniquePlayerName and UniquePlayerName.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `PlayerName` LIKE '{UniquePlayerName}' "
    elif(UniqueDoB and UniqueDoB.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `DoB` LIKE '{UniqueDoB}' "
    elif(Uniqueemail and Uniqueemail.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `Email` LIKE '{Uniqueemail}' "
    elif(UniqueWhatsApp and UniqueWhatsApp.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `WhatsApp` LIKE '{UniqueWhatsApp}' "
    elif(UniqueMembershipType and UniqueMembershipType.strip()):
        sql = f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`,`EntryDate` FROM user WHERE `MembershipType` LIKE '{UniqueMembershipType}' "
    else:
        messagebox.showwarning("","Please fill up one of the entry")
        return
    cursor.execute(sql)
    try:
        result = cursor.fetchall();
        print(result)
        for num in range(0,7):
            setph(result[0][0],0)
            setph(result[0][1],1)
            setph(result[0][2],2)
            setph(result[0][3].strftime('%Y-%m-%d'), 3)
            setph(result[0][4],4)
            setph(str(result[0][5]), 5)
            setph(result[0][6],6)
        conn.commit()
        conn.close()
    except:
        messagebox.showwarning("","No data found!")

def clear():
    for num in range (0,7):
        setph('',(num))

def export():
    cursor.connection.ping()
    sql=f"SELECT `ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`, `EntryDate` FROM `user` ORDER BY 'ID' DESC"
    cursor.execute(sql)
    dataraw=cursor.fetchall()
    date = str(datetime.now())
    date = date.replace(' ', '_')
    date = date.replace(':','-')
    dateFinal = date[0:16]
    with open("user_"+dateFinal+".csv",'a',newline='') as f:
        w = csv.writer(f,dialect='excel')
        for record in dataraw:
            w.writerow(record)
    print("sace: user_"+dateFinal+".csv")
    conn.commit()
    conn.close()
    messagebox.showinfo("","Excel File Downloaded")


# Create a frame with a blue background for organizing widgets
frame = tkinter.Frame(window, bg="#02577A") 
frame.pack()

# Define a color for the button
btnColor = 'white'

# Create a labeled frame for managing elements within the main frame
manageFrame = tkinter.LabelFrame(frame, text="Manage", borderwidth=5)

# Set the position and layout for the manageFrame within the main frame
manageFrame.grid(row=0, column=0, sticky="w", padx=[10, 200], pady=20, ipadx=[6])

saveBtn=Button(manageFrame, text="SAVE",width=10,borderwidth=3,bg=btnColor,fg='blue', command=save)
updateBtn=Button(manageFrame, text="UPDATE",width=10,borderwidth=3,bg=btnColor,fg='blue', command=update)
deleteBtn=Button(manageFrame, text="DELETE",width=10,borderwidth=3,bg=btnColor,fg='blue', command= delete)
selectBtn=Button(manageFrame, text="SELECT",width=10,borderwidth=3,bg=btnColor,fg='blue', command=select)
findBtn=Button(manageFrame, text="FIND",width=10,borderwidth=3,bg=btnColor,fg='blue', command=find)
clearBtn=Button(manageFrame, text="CLEAR",width=10,borderwidth=3,bg=btnColor,fg='blue', command=clear)
exportBtn=Button(manageFrame, text="EXPORT EXCEL",width=10,borderwidth=3,bg=btnColor,fg='blue', command=export)

saveBtn.grid(row=0,column=0, padx=5,pady=5)
updateBtn.grid(row=0,column=1, padx=5,pady=5)
deleteBtn.grid(row=0,column=2, padx=5,pady=5)
selectBtn.grid(row=0,column=3, padx=5,pady=5)
findBtn.grid(row=0,column=4, padx=5,pady=5)
clearBtn.grid(row=0,column=5, padx=5,pady=5)
exportBtn.grid(row=0,column=6, padx=5,pady=5)

# Create a labeled frame for managing elements within the main frame
entriesFrame = tkinter.LabelFrame(frame, text="Form", borderwidth=5)

# Set the position and layout for the manageFrame within the main frame
entriesFrame.grid(row=1, column=0, sticky="w", padx=[10, 200], pady=[0,20], ipadx=[6])

IDLabel=Label(entriesFrame, text="ID", anchor="e", width=15)
UIDLabel=Label(entriesFrame, text="UID Number", anchor="e", width=15)
NameLabel=Label(entriesFrame, text="Name", anchor="e", width=15)
DoBLabel=Label(entriesFrame, text="Date of Birth", anchor="e", width=15)
emailLabel=Label(entriesFrame, text="Email Address", anchor="e", width=15)
whatsAppLabel=Label(entriesFrame, text="Phone Number", anchor="e", width=15)
membershipTypeLabel=Label(entriesFrame, text="Membership Type", anchor="e", width=15)

IDLabel.grid(row = 0, column=0, padx = 10)
UIDLabel.grid(row = 1, column=0, padx = 10)
NameLabel.grid(row = 2, column=0, padx = 10)
DoBLabel.grid(row = 3, column=0, padx = 10)
emailLabel.grid(row = 4, column=0, padx = 10)
whatsAppLabel.grid(row = 5, column=0, padx = 10)
membershipTypeLabel.grid(row = 6, column=0, padx = 10)

categoryArray=['Yearly','Monthly','Daily','Employee']

IDEntry=Entry(entriesFrame, width=50, textvariable=placeholderArray[0])
UIDEntry=Entry(entriesFrame, width=50, textvariable=placeholderArray[1])
NameEntry=Entry(entriesFrame, width=50, textvariable=placeholderArray[2])
DoBEntry=Entry(entriesFrame, width=50, textvariable=placeholderArray[3])
emailEntry=Entry(entriesFrame, width=50, textvariable=placeholderArray[4])
whatsAppEntry=Entry(entriesFrame, width=50, textvariable=placeholderArray[5])
membershipTypeCombo=ttk.Combobox(entriesFrame, width=47, textvariable=placeholderArray[6], values=categoryArray)

IDEntry.grid(row = 0, column=2, padx = 5, pady=5)
UIDEntry.grid(row = 1, column=2, padx = 5, pady=5)
NameEntry.grid(row = 2, column=2, padx = 5, pady=5)
DoBEntry.grid(row = 3, column=2, padx = 5, pady=5)
emailEntry.grid(row = 4, column=2, padx = 5, pady=5)
whatsAppEntry.grid(row = 5, column=2, padx = 5, pady=5)
membershipTypeCombo.grid(row = 6, column=2,padx = 5, pady=5)

generateIDBtn = Button(entriesFrame, text="GENERATE ID", borderwidth=3,bg=btnColor,fg='blue',width = 10, command=generateID)
generateIDBtn.grid(row=0,column=3,padx=5,pady=5)

generateUIDBtn = Button(entriesFrame, text="READ RFID", borderwidth=3,bg=btnColor,fg='blue', width = 10, command=read_RFID)
generateUIDBtn.grid(row=1,column=3,padx=5,pady=5)

style.configure(window)

my_tree['column']=("ID","UID","PlayerName","DoB","Email","WhatsApp","MembershipType","EntryDate")

my_tree.column("#0", width=0, stretch=NO)
my_tree.column("ID", anchor=W, width=60)
my_tree.column("UID", anchor=W, width=60)
my_tree.column("PlayerName", anchor=W, width=80)
my_tree.column("DoB", anchor=W, width=70)
my_tree.column("Email", anchor=W, width=170)
my_tree.column("WhatsApp", anchor=W, width=100)
my_tree.column("MembershipType", anchor=W, width=110)
my_tree.column("EntryDate", anchor=W, width=80)

my_tree.heading("ID", text = "ID", anchor=W)
my_tree.heading("UID", text = "UID", anchor=W)
my_tree.heading("PlayerName", text = "PlayerName", anchor=W)
my_tree.heading("DoB", text = "DoB", anchor=W)
my_tree.heading("Email", text = "Email", anchor=W)
my_tree.heading("WhatsApp", text = "WhatsApp", anchor=W)
my_tree.heading("MembershipType", text = "MembershipType", anchor=W)
my_tree.heading("EntryDate", text = "EntryDate", anchor=W)

my_tree.tag_configure('orow',background="#EEEEEE")
my_tree.pack()
refreshTable()
# Make the window non-resizable
window.resizable(False, False)

# Start the main event loop
window.mainloop()
