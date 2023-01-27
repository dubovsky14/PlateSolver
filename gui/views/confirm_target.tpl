% include('header.tpl')

<center>

    % if target_found:
    Target found:<br>
    <div class="container" style="width:50%">
        <table class='table table-sm result-table'>
            <tbody>
                <tr>
                        <td> Right ascension </td>
                        <td> {{target_ra}} </td>
                </tr>
                <tr>
                        <td> Declination </td>
                        <td> {{target_dec}}    </td>
                </tr>
            </tbody>
        </table>
    </div>

    <a href="/?target_ra={{target_ra}}&target_dec={{target_dec}}" style="padding-bottom: 3rem">
        <input type="button" value="Confirm target" />
    </a>
    % else:
    Target not found!<br>
    % end



    <a href="/select_target">
        <input type="button" value="Go back" />
    </a>

</center>

% include('footer.tpl')